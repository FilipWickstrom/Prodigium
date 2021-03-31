#include "Game.h"

Game::Game()
{
	isRunning = true;
	device = 0;
	context = 0;
}

Game::~Game()
{
}

void Game::run()
{
	// Active game loop.
	if (isRunning)
	{

	}
}

// Result will be the message processing, depends on the message sent
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	default:
		break;
	}

	// Handle any messages the switch statement didn't by using default methodology
	return DefWindowProc(hWnd, message, wParam, lParam);
}

bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window)
{
	const wchar_t CLASS_NAME[] = L"Test Window Class";

	// Window class attributes
	WNDCLASS wc = { };

	wc.lpfnWndProc = WindowProc; //Pointer to our window procedure
	wc.hInstance = instance; // Handle of our instance which contains window procedure
	wc.lpszClassName = CLASS_NAME; // Specifies the window class name

	RegisterClass(&wc); // Registers our window class for subsequent use in calls

	// Creates our window
	window = CreateWindowEx(0, CLASS_NAME, L"This is not the greatest game in the world, This is a tribute.", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, width, height, nullptr, nullptr, instance, nullptr);

	// Error handling
	if (window == nullptr)
	{
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		return false;
	}

	// Display
	ShowWindow(window, nCmdShow);
	return true;
}
