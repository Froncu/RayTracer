#include "Renderer.h"

#include <execution>
#include <iostream>

#include "Scene.h"
#include "Materials.hpp"
#include "Utilities.hpp"

Renderer::Renderer(SDL_Window* const pWindow, const Scene* const pScene) :
	m_pWindow{ pWindow },
	m_pBuffer{ SDL_GetWindowSurface(pWindow) },
	m_pBufferPixels{ static_cast<uint32_t*>(m_pBuffer->pixels) },

	m_pScene{ pScene },

	m_LightingMode{ LightingMode::combined },

	m_CastShadows{ true },

	m_PixelsY{}

#ifdef REFLECT
	,m_ReflectionBounceAmount{ 5 },

	m_vAccumulatedReflectionData{},
	m_FrameIndex{ 1 }
#endif
{
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);

	for (float pixelY{ 0.5f }; pixelY < m_Height; ++pixelY)
		m_PixelsY.push_back(pixelY);

#ifdef REFLECT
	m_vAccumulatedReflectionData.resize(m_Width * m_Height);
#endif
}

void Renderer::Render()
{
	const Camera& camera{ m_pScene->GetCamera() };
	const auto& vpMaterials{ m_pScene->GetMaterials() };
	const auto& vLights{ m_pScene->GetLights() };

	const float
		fieldOfViewValue{ camera.GetFieldOfViewValue() },
		aspectRatioTimesFieldOfViewValue{ float(m_Width) / m_Height * fieldOfViewValue },
		multiplierXValue{ 2.0f / m_Width },
		multiplierYValue{ 2.0f / m_Height };

	const Vector3& cameraOrigin{ camera.GetOrigin() };
	const Matrix& cameraToWorld{ camera.GetCameraToWorld() };

	std::for_each(std::execution::par, m_PixelsY.begin(), m_PixelsY.end(),
		[this, &vpMaterials, &vLights, fieldOfViewValue, aspectRatioTimesFieldOfViewValue, multiplierXValue, multiplierYValue, &cameraOrigin, &cameraToWorld]
		(float py)
		{
			Vector3 rayDirection;
			rayDirection.z = 1.0f;
			rayDirection.y = (1.0f - py * multiplierYValue) * fieldOfViewValue;

			for (float px{ 0.5f }; px < m_Width; ++px)
			{
				rayDirection.x = (px * multiplierXValue - 1.0f) * aspectRatioTimesFieldOfViewValue;

				const int currentPixelIndex{ int(px) + (int(py) * m_Width) };

				Ray viewRay;
				viewRay.origin = cameraOrigin;
				viewRay.direction = cameraToWorld.TransformVector(rayDirection.GetNormalized());

				ColorRGB finalColor{};
#ifdef REFLECT
				float colorFragmentLeftToUse{ 1.0f };
				for (int reflectionBounceAmount{ 0 }; reflectionBounceAmount <= m_ReflectionBounceAmount; ++reflectionBounceAmount)
				{
#endif
					HitRecord closestHit;
					m_pScene->GetClosestHit(viewRay, closestHit);
					if (closestHit.didHit)
					{
						const Material* const pHitMaterial{ vpMaterials[closestHit.materialIndex] };
#ifdef REFLECT
						const float colorFragmentUsed{ colorFragmentLeftToUse * pHitMaterial->m_Roughness };
						colorFragmentLeftToUse -= colorFragmentUsed;
#endif
						for (const Light& light : vLights)
						{
							const Vector3 lightVector{ GetDirectionToLight(light, closestHit.origin) };
							const float lightVectorMagnitude{ lightVector.GetMagnitude() };
							const Vector3 lightVectorNormalized{ lightVector / lightVectorMagnitude };

							Ray lightRay{ closestHit.origin + RAY_EPSILON * lightVectorNormalized, lightVectorNormalized };
							lightRay.max = lightVectorMagnitude;

							if (m_CastShadows && m_pScene->DoesHit(lightRay))
								continue;

							const float dotLightDirectionNormal{ std::max(Vector3::Dot(lightRay.direction, closestHit.normal), 0.0f) };
							const ColorRGB radiance{ GetRadiance(light, closestHit.origin) };
							const ColorRGB BRDF{ pHitMaterial->Shade(closestHit, lightRay.direction, viewRay.direction) };

							switch (m_LightingMode)
							{
							case Renderer::LightingMode::observedArea:
								finalColor +=
#ifdef REFLECT
									colorFragmentUsed *
#endif
									dotLightDirectionNormal * WHITE;
								break;

							case Renderer::LightingMode::radiance:
								finalColor +=
#ifdef REFLECT
									colorFragmentUsed *
#endif
									radiance;
								break;

							case Renderer::LightingMode::BRDF:
								finalColor +=
#ifdef REFLECT
									colorFragmentUsed *
									BRDF.GetMaxToOne();
#else
									BRDF;
#endif
								break;

							case Renderer::LightingMode::combined:
								finalColor +=
									dotLightDirectionNormal *
									radiance *
#ifdef REFLECT
									colorFragmentUsed *
									BRDF.GetMaxToOne();
#else
									BRDF;
#endif
								break;
							}
						}
#ifdef REFLECT
						if (colorFragmentLeftToUse >= FLT_EPSILON)
						{
							viewRay.direction = (Vector3::Reflect(viewRay.direction, closestHit.normal) + pHitMaterial->m_Roughness * Vector3::GetRandom(-0.2f, 0.2f)).GetNormalized();
							viewRay.origin = closestHit.origin;
						}
						else
							break;
#endif
					}
#ifdef REFLECT
				}

				m_vAccumulatedReflectionData[currentPixelIndex] += finalColor;
				finalColor = m_vAccumulatedReflectionData[currentPixelIndex] / float(m_FrameIndex);
#endif
				finalColor.MaxToOne();

				m_pBufferPixels[currentPixelIndex] = SDL_MapRGB(m_pBuffer->format,
					static_cast<uint8_t>(finalColor.red * 255),
					static_cast<uint8_t>(finalColor.green * 255),
					static_cast<uint8_t>(finalColor.blue * 255));
			}
		});

#ifdef REFLECT
	++m_FrameIndex;
#endif

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
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n" 
			<< "LIGHTING MODE: Observed Area\n"
			<< "--------\n";
		break;

	case Renderer::LightingMode::radiance:
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n"
			<< "LIGHTING MODE: Radiance\n"
			<< "--------\n";
		break;

	case Renderer::LightingMode::BRDF:
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n"
			<< "LIGHTING MODE: BRDF\n"
			<< "--------\n";
		break;

	case Renderer::LightingMode::combined:
		system("CLS");
		std::cout
			<< CONTROLS
			<< "--------\n"
			<< "LIGHTING MODE: Combined\n"
			<< "--------\n";
		break;
	}

#ifdef REFLECT
	ResetAccumulatedReflectionData();
#endif
}

void Renderer::ToggleShadows()
{
	m_CastShadows = !m_CastShadows;
	system("CLS");
	std::cout
		<< CONTROLS 
		<< "--------\n"
		<< "SHADOWS: " << std::boolalpha << m_CastShadows << std::endl
		<< "--------\n";

#ifdef REFLECT
	ResetAccumulatedReflectionData();
#endif
}

#ifdef REFLECT
void Renderer::IncrementReflectionBounceAmount(int incrementer)
{
	m_ReflectionBounceAmount = std::max(m_ReflectionBounceAmount + incrementer, 1);
	system("CLS");
	std::cout
		<< CONTROLS
		<< "--------\n"
		<< "REFLECTIONS BOUNCE AMOUNT: " << m_ReflectionBounceAmount << std::endl
		<< "--------\n";

	ResetAccumulatedReflectionData();
}
#endif