#include "Renderer.h"

#include <execution>
#include <iostream>

#include "Scene.h"
#include "Materials.hpp"
#include "Utilities.hpp"

Renderer::Renderer(SDL_Window* const pWindow) :
	m_pWindow{ pWindow },
	m_pBuffer{ SDL_GetWindowSurface(pWindow) },
	m_pBufferPixels{ static_cast<uint32_t*>(m_pBuffer->pixels) },

	m_LightingMode{ LightingMode::combined },

	m_ReflectionBounceAmount{ 1 },

	m_CastShadows{ true },
	m_Reflect{ true },

	m_PixelsX{}
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	for (float pixelX{ 0.5f }; pixelX < m_Width; ++pixelX)
		m_PixelsX.push_back(pixelX);
}

void Renderer::Render(const Scene* const pScene) const
{
	const Camera& camera = pScene->GetCamera();
	const auto& vpMaterials{ pScene->GetMaterials() };
	const auto& vLights{ pScene->GetLights() };

	const float
		fieldOfViewValue{ camera.GetFieldOfViewValue() },
		aspectRatioTimesFieldOfViewValue{ float(m_Width) / m_Height * fieldOfViewValue },
		multiplierXValue{ 2.0f / m_Width },
		multiplierYValue{ 2.0f / m_Height };

	const Vector3& cameraOrigin{ camera.GetOrigin() };
	const Matrix& cameraToWorld{ camera.CalculateCameraToWorld() };

	const int maxReflectionBounceAmount{ m_Reflect ? m_ReflectionBounceAmount : 0 };

	std::for_each(std::execution::par, m_PixelsX.begin(), m_PixelsX.end(),
		[this, pScene, &vpMaterials, &vLights, fieldOfViewValue, aspectRatioTimesFieldOfViewValue, multiplierXValue, multiplierYValue, &cameraOrigin, &cameraToWorld, maxReflectionBounceAmount]
		(float px)
		{
			Vector3 rayDirection;
			rayDirection.z = 1.0f;
			rayDirection.x = (px * multiplierXValue - 1.0f) * aspectRatioTimesFieldOfViewValue;

			for (float py{ 0.5f }; py < m_Height; ++py)
			{
				rayDirection.y = (1.0f - py * multiplierYValue) * fieldOfViewValue;

				Ray viewRay;
				viewRay.origin = cameraOrigin;
				viewRay.direction = cameraToWorld.TransformVector(rayDirection.GetNormalized());

				ColorRGB finalColor{};
				float colorFragmentLeftToUse{ 1.0f };
				for (int reflectionBounceAmount{ 0 }; reflectionBounceAmount <= maxReflectionBounceAmount; ++reflectionBounceAmount)
				{
					HitRecord closestHit;
					pScene->GetClosestHit(viewRay, closestHit);
					if (closestHit.didHit)
					{
						const Material* const pHitMaterial{ vpMaterials[closestHit.materialIndex] };
						const float colorFragmentUsed{ m_Reflect ? (colorFragmentLeftToUse * pHitMaterial->m_Roughness) : 1.0f };
						if (m_Reflect)
							colorFragmentLeftToUse -= colorFragmentUsed;

						for (const Light& light : vLights)
						{
							const Vector3 lightVector{ GetDirectionToLight(light, closestHit.origin) };
							const float lightVectorMagnitude{ lightVector.GetMagnitude() };
							const Vector3 lightVectorNormalized{ lightVector / lightVectorMagnitude };

							Ray lightRay{ closestHit.origin + RAY_EPSILON * lightVectorNormalized, lightVectorNormalized };
							lightRay.max = lightVectorMagnitude;

							if (m_CastShadows && pScene->DoesHit(lightRay))
								continue;

							const float dotLightDirectionNormal{ std::max(Vector3::Dot(lightRay.direction, closestHit.normal), 0.0f) };

							switch (m_LightingMode)
							{
							case Renderer::LightingMode::observedArea:
								finalColor +=
									colorFragmentUsed *
									dotLightDirectionNormal * WHITE;
								break;

							case Renderer::LightingMode::radiance:
								finalColor +=
									colorFragmentUsed *
									GetRadiance(light, closestHit.origin);
								break;

							case Renderer::LightingMode::BRDF:
								finalColor +=
									colorFragmentUsed *
									pHitMaterial->Shade(closestHit, lightRay.direction, viewRay.direction);
								break;

							case Renderer::LightingMode::combined:
								finalColor +=
									colorFragmentUsed *
									dotLightDirectionNormal *
									GetRadiance(light, closestHit.origin) *
									pHitMaterial->Shade(closestHit, lightRay.direction, viewRay.direction);
								break;
							}
						}

						if (colorFragmentLeftToUse <= FLT_EPSILON)
							break;

						viewRay.direction = Vector3::Reflect(viewRay.direction, closestHit.normal);
						viewRay.origin = closestHit.origin;
					}
				}

				//Update Color in Buffer
				finalColor.MaxToOne();

				m_pBufferPixels[int(px) + (int(py) * m_Width)] = SDL_MapRGB(m_pBuffer->format,
					static_cast<uint8_t>(finalColor.red * 255),
					static_cast<uint8_t>(finalColor.green * 255),
					static_cast<uint8_t>(finalColor.blue * 255));
			}
		});

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
	case Renderer::LightingMode::observedArea:
		std::cout << "--------\n" << "LIGHTING MODE: Observed Area\n" << "--------\n";
			break;

	case Renderer::LightingMode::radiance:
		std::cout << "--------\n" << "LIGHTING MODE: Radiance\n" << "--------\n";
			break;

	case Renderer::LightingMode::BRDF:
		std::cout << "--------\n" << "LIGHTING MODE: BRDF\n" << "--------\n";
			break;

	case Renderer::LightingMode::combined:
		std::cout << "--------\n" << "LIGHTING MODE: Combined\n" << "--------\n";
		break;
	}
}

void Renderer::ToggleShadows()
{
	m_CastShadows = !m_CastShadows;
	std::cout << "--------\n" << "SHADOWS: " << std::boolalpha << m_CastShadows << "\n--------\n";
}

void Renderer::ToggleReflections()
{
	m_Reflect = !m_Reflect;
	std::cout << "--------\n" << "REFLECTIONS: " << std::boolalpha << m_Reflect << "\n--------\n";
}

void Renderer::IncrementReflectionBounceAmount(int incrementer)
{
	m_ReflectionBounceAmount = std::max(m_ReflectionBounceAmount + incrementer, 1);
	std::cout << "--------\n" << "REFLECTIONS BOUNCE AMOUNT: " << m_ReflectionBounceAmount << "\n--------\n";
}