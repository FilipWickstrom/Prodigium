#include <iostream>
#include <errno.h>
#include <Windows.h>
#include <stdlib.h>
//#include "Game.h"
#pragma comment(lib, "Winmm.lib")
#define WIDTH 1920
#define HEIGHT 1080

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	HWND window;
	Game game;
	if (!game.SetupWindow(hInstance, WIDTH, HEIGHT, nCmdShow, window))
	{
		std::cerr << "Setting up Window failed" << std::endl;
		return -1;
	}

	/*
		Why are you running?
	*/

	game.run();

	std::cout << "Hello World\n";
	return 0;
}