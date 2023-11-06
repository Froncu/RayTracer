#pragma once

//#include <cstdint>
#include <vector>

#include "SDL.h"

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

	void Render(const Scene* const pScene) const;
	bool SaveBufferToImage() const;

	void CycleLightingMode();
	void ToggleShadows();
	void ToggleReflections();
	void IncrementReflectionBounceAmount(int incrementer);

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
};