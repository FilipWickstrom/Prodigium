#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include "Game.h"
#include <omp.h>

#pragma comment(lib, "Winmm.lib")


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Game game(hInstance, 1920, 1080);
	game.OnStart();
	double currentFrame = 0.f, lastFrame = omp_get_wtime();
	float deltaTime = 0;
	/*
		Width and Height of presented window, Can be changed in options?
	*/

	/*game.Run(hInstance, WIDTH, HEIGHT);*/
	const float targetDelta = 1 / 120.f;
	float deltaSum = 0.f;

	while (game.IsRunning())
	{
		currentFrame = omp_get_wtime();
		deltaTime = static_cast<float>(currentFrame - lastFrame);
		InputHandler::HandleMessages();
		if (deltaSum >= targetDelta)
		{
			game.OnFrame(deltaSum);
			deltaSum = 0.f;
		}
		deltaSum += deltaTime;
		lastFrame = currentFrame;
	}

	return 0;
}