#pragma once

#include <vector>

#include "SDL.h"
#include "ColorRGB.hpp"

class Scene;

class Renderer final
{
public:
	Renderer(SDL_Window* const pWindow);
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
		if (m_Reflect)
		{
			m_FrameIndex = 1;
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

	enum class LightingMode
	{
		observedArea,
		radiance,
		BRDF,
		combined,

		AMOUNT
	} m_LightingMode;

	int m_ReflectionBounceAmount;

	bool
		m_CastShadows,
		m_Reflect;

	std::vector<float> m_PixelsX;

	std::vector<ColorRGB> m_vAccumulatedReflectionData;
	int m_FrameIndex;
};