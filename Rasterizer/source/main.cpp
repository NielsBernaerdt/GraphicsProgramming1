#include "pch.h"
//#undef main

#ifdef _DEBUG
#include <vld.h>
#endif

//Standard includes
#include <iostream>

//Project includes
#include "SceneManager.h"
#include "SceneDualRasterizer.h"
#include "ETimer.h"
#include "ERenderer.h"
#include "Camera.h"
#include "LightManager.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

void StopRotating(bool& b);
void PrintInstructions();

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
		"Dual Rasterizer - Niels Bernaerdt (2DAE07)",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);

	if (!pWindow)
		return 1;

	//Initialize "framework"
	auto pTimer{ std::make_unique<Elite::Timer>() };
	auto pCamera{ new Camera() };
	auto pRenderer{ std::make_unique<Elite::Renderer>(pWindow, pCamera) };
	LightManager::Create();
	SceneManager::Create();
	SceneManager::Get()->AddGameScene(new SceneDualRasterizer(pRenderer->GetDevice()));
	PrintInstructions();

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool pauseUpdate{ false };

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
				if (e.key.keysym.scancode == SDL_SCANCODE_E)
					pRenderer->SwapRasterizer();
				if (e.key.keysym.scancode == SDL_SCANCODE_R)
					StopRotating(pauseUpdate);
				if (e.key.keysym.scancode == SDL_SCANCODE_C)
					pRenderer->SwitchCullMode();
				if (e.key.keysym.scancode == SDL_SCANCODE_F)
					SceneManager::Get()->GetActiveScene()->SwitchSamplingState();
				if (e.key.keysym.scancode == SDL_SCANCODE_T)
					SceneManager::Get()->GetActiveScene()->ToggleDisabilityObject(1);
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
		//--------- Update ---------
		if(pauseUpdate == false)
			SceneManager::Get()->Update(pTimer->GetElapsed());
		//--------- Render ---------
		pRenderer->Render();
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pCamera;
	SceneManager::Destroy();
	LightManager::Destroy();

	ShutDown(pWindow);
	return 0;
}

void StopRotating(bool& b)
{
	b = !b;
	if (b)
		std::cout << "Rotation turned on.  \t\t\tPress 'R' to turn them on.\n";
	else
		std::cout << "Rotation turned off. \t\t\tPress 'R' to turn them on.\n";
}
void PrintInstructions()
{
	std::cout << "Toggle DirectX: \t\t\t'E'\n";
	std::cout << "Toggle Rotation:\t\t\t'R'\n";
	std::cout << "Toggle Cull Modes:\t\t\t'C'\n";
	std::cout << "Toggle Texture Sampling states:\t\t'F'\n";
	std::cout << "Toggle FireFX mesh:              \t'T'\n";
}