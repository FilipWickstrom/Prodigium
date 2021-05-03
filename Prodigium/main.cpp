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
	game.OnStart();
	double currentFrame = 0.f, lastFrame = omp_get_wtime();
	float deltaTime = 0.f, deltaSum = 0.f;
	const float targetDelta = 1 / 120.f;
	const float refreshTime = 1.0f;
	float timeCounter = 0.0f;
	float lastFrameRate = 0.0f;
	int frameCounter = 0;

	while (game.IsRunning())
	{
		currentFrame = omp_get_wtime();
		deltaTime = static_cast<float>(currentFrame - lastFrame);
		InputHandler::HandleMessages();
		// THIS TO SEE FRAMERATE
		if (timeCounter < refreshTime)
		{
			timeCounter += deltaTime;
		}
		else
		{
			lastFrameRate = (float)frameCounter / timeCounter;
			//printf("FPS: %f\r", lastFrameRate);
			frameCounter = 0;
			timeCounter = 0.f;
		}
		// END OF FRAMERATE
		if (deltaSum >= targetDelta)
		{
			game.OnFrame(deltaSum);
			deltaSum = 0.f;
			frameCounter++;;
		}
		deltaSum += deltaTime;
		lastFrame = currentFrame;
	}

	return 0;
}