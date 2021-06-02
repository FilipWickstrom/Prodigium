#include "Window.h"
#include "InputHandler.h"
bool Window::SetupWindow(const HINSTANCE& instance, const UINT& width, const UINT& height)
{
	this->WIDTH = width;
	this->HEIGHT = height;

	const wchar_t CLASS_NAME[] = L"Prodigium Class";

	int posX, posY;

	WNDCLASS wc = { };
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.lpfnWndProc = InputHandler::WindowProc;
	wc.hInstance = instance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	posX = (GetSystemMetrics(SM_CXSCREEN) - width) / 2;
	posY = (GetSystemMetrics(SM_CYSCREEN) - height) / 2;

	windowHandler = CreateWindowEx(0,
		CLASS_NAME,
		L"Prodigium",
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

	//ShowWindow(windowHandler, SW_SHOW);
	//Fullscreen borderless mode
	// Tries to maximize the screen with set resolution from main
	ShowWindow(windowHandler, SW_MAXIMIZE);
	SetWindowLongPtr(windowHandler, GWL_STYLE, WS_VISIBLE | WS_POPUP);
	int w = GetSystemMetrics(SM_CXSCREEN);
	int h = GetSystemMetrics(SM_CYSCREEN);
	SetWindowPos(windowHandler, HWND_TOP, 0, 0, w, h, SWP_FRAMECHANGED);
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

