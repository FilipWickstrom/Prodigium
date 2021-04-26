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
	Game game(hInstance, 1280, 1024);
	float currentFrame, lastFrame, deltaTime = 0;
	/*
		Width and Height of presented window, Can be changed in options?
	*/

	/*game.Run(hInstance, WIDTH, HEIGHT);*/

	while (game.IsRunning())
	{
		InputHandler::HandleMessages();
		currentFrame = static_cast<float>(omp_get_wtime());
		game.OnFrame(deltaTime);
		lastFrame = static_cast<float>(omp_get_wtime());
		deltaTime = lastFrame - currentFrame;
	}

	return 0;
}