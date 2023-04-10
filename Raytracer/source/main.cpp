//External includes
#include "vld.h"
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "ReferenceScene.h"
#include "BunnyScene.h"
#include "SceneManager.h"

void PrintInstructions();
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
	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - Niels Bernaerdt (2DAE07)",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	PrintInstructions();
	SceneManager::Create();
	SceneManager::Get()->AddGameScene(new ReferenceScene());
	SceneManager::Get()->AddGameScene(new BunnyScene());
	Elite::Timer* pTimer = new Elite::Timer();
	Camera* pCamera = new Camera();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow, pCamera);

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool shouldRotate = true;
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
				if(e.key.keysym.scancode == SDL_SCANCODE_X)
					takeScreenshot = true;
				if (e.key.keysym.scancode == SDL_SCANCODE_F4)
				{
					SceneManager::Get()->NextScene();
				}
				if (e.key.keysym.scancode == SDL_SCANCODE_Q)
					pRenderer->ToggleShadows();
				if (e.key.keysym.scancode == SDL_SCANCODE_E)
					pRenderer->ToggleLightEquation();
				if (e.key.keysym.scancode == SDL_SCANCODE_C)
					pRenderer->ToggleAcceleratedStructures();
				if (e.key.keysym.scancode == SDL_SCANCODE_R)
				{
					shouldRotate = !shouldRotate;
					if(shouldRotate)
						std::cout << "Rotation turned on.                             Press 'R' to disable Rotations.\n";
					else
						std::cout << "Rotation turned off.                            Press 'R' to enable Rotations.\n";
				}
				break;
			}
		}

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer = 0.f;
			std::cout << "FPS: " << pTimer->GetFPS() << std::endl;
		}

		//--------- Camera ---------
		pCamera->Update(pTimer->GetElapsed());

		//--------- Rotate ---------
		if (shouldRotate == true)
		{
			for (const auto& object : SceneManager::Get()->GetActiveScene()->GetObjects())
				object->Rotate(pTimer->GetElapsed());
		}

		//--------- Render ---------
		pRenderer->Render();

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;
	delete pCamera;
	SceneManager::Destroy();

	ShutDown(pWindow);
	return 0;
}

void PrintInstructions()
{
	std::cout << "Switch scene:\t\t\t'F4'\n";
	std::cout << "Toggle Shadow:\t\t\t'Q'\n";
	std::cout << "Toggle light equation:\t\t'E'\n";
	std::cout << "Toggle acceleration structures:\t'C'\n";
	std::cout << "Toggle Rotations:              \t'R'\n";
}