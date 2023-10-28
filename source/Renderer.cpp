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

	const int maxReflectionBounceAmount{ m_Reflect ? m_ReflectionBounceAmount : 0 };

	Vector3 rayDirection;
	Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
	Ray viewRay;

	rayDirection.z = 1.0f;

	for (float px{ 0.5f }; px < m_Width; ++px)
	{
		rayDirection.x = (px * multiplierXValue - 1.0f) * aspectRatioTimesFieldOfViewValue;

		for (float py{ 0.5f }; py < m_Height; ++py)
		{
			rayDirection.y = (1.0f - py * multiplierYValue) * fieldOfViewValue;

			viewRay.origin = camera.origin;
			viewRay.direction = cameraToWorld.TransformVector(rayDirection.Normalized());
			
			ColorRGB finalColor{};
			float colorFragmentLeftToUse{ 1.0f };
			for (int reflectionBounceAmount{ 0 }; reflectionBounceAmount <= maxReflectionBounceAmount; ++reflectionBounceAmount)
			{
				if (colorFragmentLeftToUse <= FLT_EPSILON)
					break;

				HitRecord closestHit;
				pScene->GetClosestHit(viewRay, closestHit);
				if (closestHit.didHit)
				{
					const Material* const pHitMaterial{ materials[closestHit.materialIndex] };
					const float colorFragmentUsed{ m_Reflect ? (colorFragmentLeftToUse * pHitMaterial->m_Roughness) : 1.0f };
					if (m_Reflect) 
						colorFragmentLeftToUse -= colorFragmentUsed;

					for (const Light& light : lights)
					{
						const Vector3 lightVector{ dae::LightUtils::GetDirectionToLight(light, closestHit.origin) };
						const float lightVectorMagnitude{ lightVector.Magnitude() };
						const Vector3 lightVectorNormalized{ lightVector / lightVectorMagnitude };

						Ray lightRay{ closestHit.origin + RAY_EPSILON * lightVectorNormalized, lightVectorNormalized };
						lightRay.max = lightVectorMagnitude;

						if (m_ShowShadows && pScene->DoesHit(lightRay))
							continue;

						const float dotLightDirectionNormal{ std::max(Vector3::Dot(lightRay.direction, closestHit.normal), 0.0f) };

						switch (m_LightingMode)
						{
						case dae::Renderer::LightingMode::observedArea:
							finalColor +=
								colorFragmentUsed *
								dotLightDirectionNormal * colors::White;
							break;

						case dae::Renderer::LightingMode::radiance:
							finalColor +=
								colorFragmentUsed *
								LightUtils::GetRadiance(light, closestHit.origin);
							break;

						case dae::Renderer::LightingMode::BRDF:
							finalColor +=
								colorFragmentUsed *
								pHitMaterial->Shade(closestHit, lightRay.direction, viewRay.direction);
							break;

						case dae::Renderer::LightingMode::combined:
							finalColor +=
								colorFragmentUsed *
								dotLightDirectionNormal *
								LightUtils::GetRadiance(light, closestHit.origin) *
								pHitMaterial->Shade(closestHit, lightRay.direction, viewRay.direction);
							break;
						}
					}

					viewRay.direction = Vector3::Reflect(viewRay.direction, closestHit.normal);
					viewRay.origin = closestHit.origin;
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

	switch (m_LightingMode)
	{
	case dae::Renderer::LightingMode::observedArea:
		std::cout << "--------\n" << "LIGHTING MODE: Observed Area\n" << "--------\n";
			break;

	case dae::Renderer::LightingMode::radiance:
		std::cout << "--------\n" << "LIGHTING MODE: Radiance\n" << "--------\n";
			break;

	case dae::Renderer::LightingMode::BRDF:
		std::cout << "--------\n" << "LIGHTING MODE: BRDF\n" << "--------\n";
			break;

	case dae::Renderer::LightingMode::combined:
		std::cout << "--------\n" << "LIGHTING MODE: Combined\n" << "--------\n";
		break;
	}
}

void Renderer::ToggleShadows()
{
	m_ShowShadows = !m_ShowShadows;
	std::cout << "--------\n" << "SHADOWS: " << std::boolalpha << m_ShowShadows << "\n--------\n";
}

void dae::Renderer::ToggleReflections()
{
	m_Reflect = !m_Reflect;
	std::cout << "--------\n" << "REFLECTIONS: " << std::boolalpha << m_Reflect << "\n--------\n";
}

void dae::Renderer::IncrementReflectionBounceAmount(int incrementer)
{
	m_ReflectionBounceAmount = std::max(m_ReflectionBounceAmount + incrementer, 1);
	std::cout << "--------\n" << "REFLECTIONS BOUNCE AMOUNT: " << m_ReflectionBounceAmount << "\n--------\n";
}