#include <iostream>
#include <errno.h>
#include <stdlib.h>
#include "Game.h"
#include <omp.h>

#pragma comment(lib, "Winmm.lib")

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
	const wchar_t CLASS_NAME[] = L"Test Window Class";

	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	window = CreateWindowEx(0, CLASS_NAME, L"Prodigium", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);

	if (window == nullptr)
	{
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		return false;
	}

	ShowWindow(window, nCmdShow);
	return true;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	Game game;
	float currentFrame, lastFrame, deltaTime = 0;
	/*
		Width and Height of presented window, Can be changed in options?
	*/
	const int WIDTH = 1280;
	const int HEIGHT = 1024;

	game.Run(hInstance, WIDTH, HEIGHT);

	MSG state = {};
	while (!(GetKeyState(VK_ESCAPE) & 0x80) && state.message != WM_QUIT)
	{
		if (PeekMessage(&state, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&state);
			DispatchMessage(&state);
		}
		// FOR TESTING
		ResourceManager::GetTexture("Textures/diffus.tga");
		ResourceManager::GetTexture("Textures/diffus.tga");
		ResourceManager::GetTexture("Textures/diffus.tga");
		ResourceManager::GetTexture("Textures/diffus.tga");
		ResourceManager::GetTexture("Textures/diffus.tga");
		ResourceManager::GetTexture("Textures/diffus.tga");
		ResourceManager::GetTexture("Textures/diffus.tga");

		currentFrame = static_cast<float>(omp_get_wtime());
		game.OnFrame(deltaTime);
		lastFrame = static_cast<float>(omp_get_wtime());
		deltaTime = lastFrame - currentFrame;
	}

	return 0;
}