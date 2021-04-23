#pragma once
#include "UsefulHeader.h"
#include "Graphics.h"
#include <keyboard.h>
#include <Mouse.h>
using namespace DirectX;
class InputHandler
{
private:
	InputHandler();
	virtual ~InputHandler();
	std::unique_ptr<Keyboard> keyboard;
	std::unique_ptr<Mouse> mouse;
	std::unique_ptr<Keyboard::KeyboardStateTracker> kBTracker;
	std::unique_ptr<Mouse::ButtonStateTracker> mouseTracker;
	Keyboard::State kbState;
	Mouse::State mouseState;
	static InputHandler* instance;
	MSG state;
public:
	DELETE_COPY_ASSIGNMENT(InputHandler);
	static bool Initialize(const HWND& windowHandle);
	static int GetMouseX();
	static int GetMouseY();
	static void UpdateKeyboardAndMouse();
	static Keyboard::KeyboardStateTracker* GetKBStateTracker();
	static Mouse::ButtonStateTracker* getMouseStateTracker();
	static bool IsKeyPressed(Keyboard::Keys key);
	static bool IsKeyHeld(Keyboard::Keys key);
	static bool IsLMBPressed();
	static bool IsLMBHeld();
	static bool IsRMBPressed();
	static bool IsRMBHeld();
	static bool IsMMBPressed();
	static bool IsMMBHeld();
	static void HandleMessages();
	static LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

