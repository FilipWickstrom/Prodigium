#include "InputHandler.h"
#include "imgui_impl_win32.h" //TESTING
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
InputHandler* InputHandler::instance = nullptr;
InputHandler::InputHandler()
{
	kbState = {};
	mouseState = {};
	state = {};
	isRunning = true;
	confineCursor = false;
}
InputHandler::~InputHandler()
{
}

const bool InputHandler::Initialize()
{
	if (!InputHandler::instance)
	{
		InputHandler::instance = new InputHandler;
		InputHandler::instance->keyboard = std::make_unique<DirectX::Keyboard>();
		InputHandler::instance->kBTracker = std::make_unique<Keyboard::KeyboardStateTracker>();
		InputHandler::instance->mouse = std::make_unique<DirectX::Mouse>();
		InputHandler::instance->mouseTracker = std::make_unique<Mouse::ButtonStateTracker>();

	}
	else
	{
		std::cerr << "InputHandler already Initialized\n";
	}
	return true;

}

void InputHandler::setWindow(const HWND& windowHandle)
{
	InputHandler::instance->mouse->SetWindow(windowHandle);
	InputHandler::instance->mouse->SetMode(Mouse::MODE_RELATIVE);
}

const int InputHandler::GetMouseX()
{
	return InputHandler::instance->mouseState.x;
}
const int InputHandler::GetMouseY()
{
	return InputHandler::instance->mouseState.y;
}


void InputHandler::UpdateKeyboardAndMouse()
{
	InputHandler::instance->kbState = InputHandler::instance->keyboard->GetState();
	InputHandler::instance->kBTracker->Update(InputHandler::instance->kbState);
	InputHandler::instance->mouseState = InputHandler::instance->mouse->GetState();
	InputHandler::instance->mouseTracker->Update(InputHandler::instance->mouseState);
}

const Keyboard::KeyboardStateTracker* InputHandler::GetKBStateTracker()
{
	return InputHandler::instance->kBTracker.get();
}

const Mouse::ButtonStateTracker* InputHandler::getMouseStateTracker()
{
	return InputHandler::instance->mouseTracker.get();
}

const Mouse::Mode InputHandler::getMouseMode()
{
	return InputHandler::instance->mouseState.positionMode;
}

const bool InputHandler::IsKeyPressed(const Keyboard::Keys& key)
{
	return InputHandler::instance->kBTracker->IsKeyPressed(key);
}
const bool InputHandler::IsKeyHeld(const Keyboard::Keys& key)
{
	return InputHandler::instance->kbState.IsKeyDown(key);
}
const bool InputHandler::IsKeyReleased(const Keyboard::Keys& key)
{
	return InputHandler::instance->kBTracker->IsKeyReleased(key);
}
const bool InputHandler::IsLMBPressed()
{
	return InputHandler::instance->mouseTracker->leftButton == InputHandler::instance->mouseTracker->PRESSED;
}
const bool InputHandler::IsLMBHeld()
{
	return  InputHandler::instance->mouseTracker->leftButton == InputHandler::instance->mouseTracker->HELD;
}
const bool InputHandler::IsRMBPressed()
{
	return  InputHandler::instance->mouseTracker->rightButton == InputHandler::instance->mouseTracker->PRESSED;
}
const bool InputHandler::IsRMBHeld()
{
	return  InputHandler::instance->mouseTracker->rightButton == InputHandler::instance->mouseTracker->HELD;
}
const bool InputHandler::IsMMBPressed()
{
	return  InputHandler::instance->mouseTracker->middleButton == InputHandler::instance->mouseTracker->PRESSED;
}
const bool InputHandler::IsMMBHeld()
{
	return InputHandler::instance->mouseTracker->middleButton == InputHandler::instance->mouseTracker->HELD;
}
const bool InputHandler::IsRunning()
{
	return InputHandler::instance->isRunning;
}
void InputHandler::Destroy()
{
	if (InputHandler::instance)
	{
		delete InputHandler::instance;
	}
}
void InputHandler::HandleMessages()
{
	while (PeekMessage(&InputHandler::instance->state, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&InputHandler::instance->state);
		DispatchMessage(&InputHandler::instance->state);
	}
}

LRESULT InputHandler::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
 	if (InputHandler::instance->confineCursor)
	{
		RECT rect;
		GetClientRect(hWnd, &rect);
		MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
		ClipCursor(&rect);
	}
	else
	{
		ClipCursor(nullptr);
	}

	if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
	{
		return true;
	}
	// Sort through and find what code to run for the message given
	switch (message)
	{
	case WM_QUIT:
		break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_ACTIVATE:
		InputHandler::instance->confineCursor = true;
		break;
	case WM_ACTIVATEAPP:
		InputHandler::instance->keyboard->ProcessMessage(message, wParam, lParam);
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		InputHandler::instance->confineCursor = false;
		break;
	case WM_DESTROY:
		// This message is read when the window is closed
		// Tell calling thread to terminate
		PostQuitMessage(0);
		InputHandler::instance->isRunning = false;
		break;
	case WM_INPUT:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYDOWN:
		InputHandler::instance->keyboard->ProcessMessage(message, wParam, lParam);
		break;
	case WM_KEYUP:
		InputHandler::instance->keyboard->ProcessMessage(message, wParam, lParam);
		break;
	case WM_MOUSEMOVE:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_MBUTTONDBLCLK:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_MBUTTONUP:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_MBUTTONDOWN:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_LBUTTONDOWN:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_LBUTTONUP:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_RBUTTONDOWN:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_RBUTTONUP:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_MOUSEHOVER:
		InputHandler::instance->mouse->ProcessMessage(message, wParam, lParam);
		break;
	case WM_SYSKEYDOWN:
		InputHandler::instance->keyboard->ProcessMessage(message, wParam, lParam);
		break;
	case WM_SYSKEYUP:
		InputHandler::instance->keyboard->ProcessMessage(message, wParam, lParam);
		break;
	}

	// Handle any messages the switch statement didn't by using default methodology
	return DefWindowProc(hWnd, message, wParam, lParam);
}

