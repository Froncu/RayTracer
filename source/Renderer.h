//#define REFLECT

#pragma once

#include <vector>

#include "SDL.h"
#include "ColorRGB.hpp"

class Scene;

class Renderer final
{
public:
	Renderer(SDL_Window* const pWindow, const Scene* const pScene);
	~Renderer() = default;

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) noexcept = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) noexcept = delete;

	void Render();
	bool SaveBufferToImage() const;

	void CycleLightingMode();
	void ToggleShadows();
#ifdef REFLECT
	void IncrementReflectionBounceAmount(int incrementer);

	inline void ResetAccumulatedReflectionData()
	{
		m_FrameIndex = 1;
		m_vAccumulatedReflectionData.assign(m_Width * m_Height, ColorRGB(0.0f, 0.0f, 0.0f));
	}
#endif

private:
	SDL_Window* const m_pWindow;
	SDL_Surface* const m_pBuffer;
	uint32_t* m_pBufferPixels;

	const Scene* const m_pScene;

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

	bool m_CastShadows;

	std::vector<float> m_PixelsY;

#ifdef REFLECT
	int m_ReflectionBounceAmount;

	std::vector<ColorRGB> m_vAccumulatedReflectionData;
	int m_FrameIndex;
#endif
};