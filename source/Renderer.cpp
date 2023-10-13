//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float
		fieldOfViewValue{ camera.GetFieldOfViewValue() },
		aspectRatioTimesFieldOfViewValue{ float(m_Width) / m_Height * fieldOfViewValue },
		multiplierXValue{ 2.0f / m_Width },
		multiplierYValue{ 2.0f / m_Height };

	Vector3 rayDirection;
	Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
	Ray viewRay{ camera.origin };

	bool
		showObservedArea,
		showRadiance,
		showBRDF;

	switch (m_LightingMode)
	{
	case dae::Renderer::LightingMode::observedArea:
		showObservedArea = true;
		showRadiance = false;
		showBRDF = false;
		break;

	case dae::Renderer::LightingMode::radiance:
		showObservedArea = false;
		showRadiance = true;
		showBRDF = false;
		break;

	case dae::Renderer::LightingMode::BRDF:
		showObservedArea = false;
		showRadiance = false;
		showBRDF = true;
		break;

	case dae::Renderer::LightingMode::combined:
		showObservedArea = true;
		showRadiance = true;
		showBRDF = true;
		break;
	}

	rayDirection.z = 1.0f;

	for (float px{ 0.5f }; px < m_Width; ++px)
	{
		rayDirection.x = (px * multiplierXValue - 1.0f) * aspectRatioTimesFieldOfViewValue;

		for (float py{ 0.5f }; py < m_Height; ++py)
		{
			rayDirection.y = (1.0f - py * multiplierYValue) * fieldOfViewValue;

			viewRay.direction = cameraToWorld.TransformVector(rayDirection.Normalized());
			
			HitRecord closestHit;
			ColorRGB finalColor{};
			pScene->GetClosestHit(viewRay, closestHit);
			if (closestHit.didHit)
			{
				for (const Light& light : lights)
				{
					const Vector3
						lightVector{ dae::LightUtils::GetDirectionToLight(light, closestHit.origin) },
						lightVectorNormalized{ lightVector.Normalized() };

					Ray lightRay{ closestHit.origin + DEFAULT_RAY_MIN * lightVectorNormalized, lightVectorNormalized };
					lightRay.max = lightVector.Magnitude();

					if (m_ShowShadows && pScene->DoesHit(lightRay))
						continue;

					const float dotLightDirectionNormal{ std::max(Vector3::Dot(lightRay.direction, closestHit.normal), 0.0f) };

					finalColor += 
						colors::White * (showObservedArea ? dotLightDirectionNormal : 1.0f) *
						(showRadiance ? LightUtils::GetRadiance(light, closestHit.origin) : colors::White) *
						(showBRDF ? materials[closestHit.materialIndex]->Shade(closestHit, lightRay.direction, viewRay.direction) : colors::White);
				}
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[int(px) + (int(py) * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}

void Renderer::CycleLightingMode()
{
	m_LightingMode = LightingMode((int(m_LightingMode) + 1) % int(LightingMode::AMOUNT));
}

void Renderer::ToggleShadows()
{
	m_ShowShadows = !m_ShowShadows;
}