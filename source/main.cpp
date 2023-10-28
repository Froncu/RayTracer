//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "Timer.h"
#include "Renderer.h"
#include "Scene.h"

using namespace dae;

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

int main(int argc, char* args[])
{
	//Unreferenced parameters
	(void)argc;
	(void)args;

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;

	const std::string title{ "RayTracer - Fratczak Jakub" };
	SDL_Window* pWindow = SDL_CreateWindow(
		title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	SDL_SetRelativeMouseMode(SDL_bool(true));

	//Initialize "framework"
	const auto pTimer = new Timer();
	const auto pRenderer = new Renderer(pWindow);

	//const auto pScene = new Scene_W1();
	//const auto pScene = new Scene_W2();
	const auto pScene = new Scene_W3();
	pScene->Initialize();

	//Start loop
	pTimer->Start();

	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;
	while (isLooping)
	{
		//--------- Get input events ---------
		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;

			case SDL_KEYUP:
				switch (e.key.keysym.scancode)
				{
				case SDL_SCANCODE_X:
					takeScreenshot = true;
					break;

				case SDL_SCANCODE_UP:
					pRenderer->IncrementReflectionBounceAmount(1);
					break;

				case SDL_SCANCODE_DOWN:
					pRenderer->IncrementReflectionBounceAmount(-1);
					break;

				case SDL_SCANCODE_F1:
					pRenderer->ToggleReflections();
					break;

				case SDL_SCANCODE_F2:
					pRenderer->ToggleShadows();
					break;

				case SDL_SCANCODE_F3:
					pRenderer->CycleLightingMode();
					break;

				case SDL_SCANCODE_F6:
					pTimer->StartBenchmark();
					break;
				}
				break;

			case SDL_MOUSEWHEEL:
				pScene->GetCamera().IncrementFieldOfViewAngle(4.0f * -float(e.wheel.y));
				break;
			}
		}

		//--------- Update ---------
		pScene->Update(pTimer);

		//--------- Render ---------
		pRenderer->Render(pScene);

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			SDL_SetWindowTitle(pWindow, (title + " - dFPS: " + std::to_string(pTimer->GetdFPS())).c_str());
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pScene;
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}