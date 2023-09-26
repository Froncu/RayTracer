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

	const float aspectRatio{ float(m_Width) / m_Height },
				fieldOfView{ tanf(dae::TO_RADIANS * camera.fovAngle / 2.0f) },
				multiplierXValue{ 2.0f / m_Width },
				multiplierYValue{ 2.0f / m_Height };

	Vector3 rayDirection;

	rayDirection.z = 1.0f;

	for (float px{ 0.5f }; px < m_Width; ++px)
	{
		rayDirection.x = (px * multiplierXValue - 1.0f) * aspectRatio * fieldOfView;

		for (float py{ 0.5f }; py < m_Height; ++py)
		{
			rayDirection.y = (1.0f - py * multiplierYValue) * fieldOfView;

			Matrix cameraToWorld{ camera.CalculateCameraToWorld() };
			Ray viewRay{ camera.origin, cameraToWorld.TransformVector(rayDirection.Normalized()) };
			HitRecord closestHit{};
			ColorRGB finalColor;
			
			pScene->GetClosestHit(viewRay, closestHit);
			if (closestHit.didHit)
			{
				float colorScalar{ 1.0f };
				for (const Light& light : lights)
				{
					const Vector3 lightVector{ dae::LightUtils::GetDirectionToLight(light, closestHit.origin) };
					Ray lightRay{ closestHit.origin, lightVector.Normalized() };
					lightRay.max = lightVector.Magnitude();

					if (pScene->DoesHit(lightRay))
						colorScalar = 0.8f;
				}

				finalColor = colorScalar * materials[closestHit.materialIndex]->Shade();
			}
			else
				finalColor = {};

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