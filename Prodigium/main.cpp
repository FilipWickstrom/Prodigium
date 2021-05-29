#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include "Game.h"
#include <omp.h>
#include <crtdbg.h>

#pragma comment(lib, "Winmm.lib")


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UINT winWidth = 1720;
	UINT winHeight = 1080;
	//#ifdef _DEBUG
	winWidth = 1280;
	winHeight = 1024;
	//#endif 

	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	Game game(hInstance, winWidth, winHeight);
	game.OnStart();

	double currentFrame = 0.f, lastFrame = omp_get_wtime();
	float deltaTime = 0.f, deltaSum = 0.f;
	const float targetDelta = 1 / 144.f;
	srand((unsigned int)time(0));
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