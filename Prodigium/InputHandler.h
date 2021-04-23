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
	static InputHandler* instance;
	MSG state;
public:
	DELETE_COPY_ASSIGNMENT(InputHandler);
	static bool Initialize(const HWND& windowHandle);
	static Keyboard::State GetKBState();
	static Mouse::State GetMouseState();
	static Keyboard::KeyboardStateTracker* GetKBStateTracker();
	static void HandleMessages();
	static LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

