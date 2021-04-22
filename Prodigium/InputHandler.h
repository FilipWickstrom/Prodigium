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
	std::unique_ptr<DirectX::Keyboard> keyboard;
	std::unique_ptr<DirectX::Mouse> mouse;
	static InputHandler* instance;
public:
	DELETE_COPY_ASSIGNMENT(InputHandler);
	static bool Initialize(const HWND& windowHandle);
	static Keyboard::State getKBState();
	static Mouse::State getMouseState();
	static LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

