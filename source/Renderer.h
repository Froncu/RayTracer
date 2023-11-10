#pragma once

#include <vector>

#include "SDL.h"
#include "ColorRGB.hpp"

class Scene;

class Renderer final
{
public:
	struct Settings
	{
		enum class LightingMode
		{
			observedArea,
			radiance,
			BRDF,
			combined,

			AMOUNT
		};

		Settings(LightingMode lightingMode = LightingMode::combined, bool castShadows = true, bool reflect = false, int reflectionBounceAmount = 1) :
			m_LightingMode{ lightingMode },

			m_CastShadows{ castShadows },
			m_Reflect{ reflect },

			m_ReflectionBounceAmount{ reflectionBounceAmount }
		{
		}

		LightingMode m_LightingMode;

		bool
			m_CastShadows,
			m_Reflect;

		int m_ReflectionBounceAmount;
	};

	Renderer(SDL_Window* const pWindow, const Settings& initialSettings);
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	void Render(const Scene* const pScene);
	bool SaveBufferToImage() const;

	void CycleLightingMode();
	void ToggleShadows();
	void ToggleReflections();
	void IncrementReflectionBounceAmount(int incrementer);

	inline void ResetAccumulatedReflectionData()
	{
		if (m_Settings.m_Reflect)
		{
			m_vFrameIndices.assign(m_Width * m_Height, 1);
			m_vAccumulatedReflectionData.assign(m_Width * m_Height, ColorRGB(0.0f, 0.0f, 0.0f));
		}
	}

private:
	SDL_Window* const m_pWindow;
	SDL_Surface* const m_pBuffer;
	uint32_t* m_pBufferPixels;

	int
		m_Width,
		m_Height;

	Settings m_Settings;

	std::vector<float> m_PixelsX;

	std::vector<ColorRGB> m_vAccumulatedReflectionData;
	std::vector<int> m_vFrameIndices;
};