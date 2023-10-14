#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;

		void CycleLightingMode();
		void ToggleShadows();

	private:
		enum class LightingMode
		{
			observedArea,
			radiance,
			BRDF,
			combined,

			AMOUNT
		};

		LightingMode m_LightingMode{ LightingMode::combined };

		bool
			m_ShowObservedArea{ true },
			m_ShowRadiance{ true },
			m_ShowBRDF{ true },
			m_ShowShadows{ true };

		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};
	};
}
