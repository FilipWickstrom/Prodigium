#include "InputHandler.h"
InputHandler* InputHandler::instance = nullptr;
InputHandler::InputHandler()
{
	lol = 0;
}
InputHandler::~InputHandler()
{
	if (InputHandler::instance)
	{
		delete InputHandler::instance;
	}
}

bool InputHandler::Initialize(const HWND& windowHandle)
{
	if (!InputHandler::instance)
	{
		InputHandler::instance = new InputHandler;
		InputHandler::instance->lol = 1;
		InputHandler::instance->keyboard = std::make_unique<DirectX::Keyboard>();
		InputHandler::instance->mouse = std::make_unique<DirectX::Mouse>();
		InputHandler::instance->mouse->SetWindow(windowHandle);
	}
	else
	{
		std::cerr << "InputHandler already Initialized\n";
	}
	return true;
}

LRESULT InputHandler::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
