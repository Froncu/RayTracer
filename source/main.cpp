#include <string>
#include <iostream>

#include "vld.h"
#include "SDL.h"
#include "Timer.h"
#include "Renderer.h"
#include "Scene.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char* args[])
{
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t 
		width{ 640 },
		height{ 480 };

	const std::string title{ "RayTracer - Fratczak Jakub" };
	SDL_Window* pWindow = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	SDL_SetRelativeMouseMode(SDL_bool(true));

	Timer timer{};
	Renderer renderer{ pWindow };

	Scene* const pScene = 
		//new SceneWeek1();
		//new SceneWeek2();
		//new SceneWeek3();
		new SceneWeek4();
		//new SceneWeek4Bunny();

	timer.Start();

	bool
		isLooping{ true },
		takeScreenshot{};
	float printTimer{};
	while (isLooping)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;

			case SDL_KEYUP:
				switch (event.key.keysym.scancode)
				{
				case SDL_SCANCODE_X:
					takeScreenshot = true;
					break;

				case SDL_SCANCODE_UP:
					renderer.IncrementReflectionBounceAmount(1);
					break;

				case SDL_SCANCODE_DOWN:
					renderer.IncrementReflectionBounceAmount(-1);
					break;

				case SDL_SCANCODE_F1:
					renderer.ToggleReflections();
					break;

				case SDL_SCANCODE_F2:
					renderer.ToggleShadows();
					break;

				case SDL_SCANCODE_F3:
					renderer.CycleLightingMode();
					break;

				case SDL_SCANCODE_F6:
					timer.StartBenchmark();
					break;
				}
				break;

			case SDL_MOUSEWHEEL:
				pScene->GetCamera().IncrementFieldOfViewAngle(-float(event.wheel.y) / 20.0f);
				break;
			}
		}

		pScene->Update(timer);
		renderer.Render(pScene);
		timer.Update();
		printTimer += timer.GetElapsed();
		if (printTimer >= 1.0f)
		{
			printTimer = 0.0f;
			SDL_SetWindowTitle(pWindow, (title + " - dFPS: " + std::to_string(timer.GetdFPS())).c_str());
		}

		if (takeScreenshot)
		{
			if (!renderer.SaveBufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}

	timer.Stop();

	delete pScene;
	ShutDown(pWindow);
	return 0;
}