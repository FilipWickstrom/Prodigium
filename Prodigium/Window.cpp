#include "Window.h"

bool Window::SetupWindow(HINSTANCE& instance, UINT width, UINT height)
{
	this->WIDTH = width;
	this->HEIGHT = height;

	const wchar_t CLASS_NAME[] = L"Test Window Class";

	int posX, posY;

	WNDCLASS wc = { };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = Window::WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	windowHandler = CreateWindowEx(0,
		CLASS_NAME,
		L"TEST WINDOW",
		WS_OVERLAPPEDWINDOW,
		posX,
		posY,
		WIDTH,
		HEIGHT,
		nullptr,
		nullptr,
		instance,
		nullptr);

	if (windowHandler == nullptr)
	{
		std::cerr << "HWND was nullptr, last error: " << GetLastError() << std::endl;
		return false;
	}

	ShowWindow(windowHandler, SW_SHOW);
	SetForegroundWindow(windowHandler);
	SetFocus(windowHandler);

	return true;
}

Window::Window()
{
	this->WIDTH = 0;
	this->HEIGHT = 0;
	this->windowHandler = {};
}

Window::~Window()
{
}

const HWND& Window::GetWindowHandler() const
{
	return windowHandler;
}

const UINT& Window::GetWindowWidth() const
{
	return WIDTH;
}

const UINT& Window::GetWindowHeight() const
{
	return HEIGHT;
}

LRESULT Window::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// Sort through and find what code to run for the message given
	switch (message)
	{
		// This message is read when the window is closed
	case WM_DESTROY:
		// Tell calling thread to terminate
		PostQuitMessage(0);
		return 0;
	case WM_KEYDOWN:
		break;
	case WM_MOUSEMOVE:
		break;
	case WM_LBUTTONDOWN:
		break;
	case WM_RBUTTONDOWN:
		break;
	}

	// Handle any messages the switch statement didn't by using default methodology
	return DefWindowProc(hWnd, message, wParam, lParam);
}