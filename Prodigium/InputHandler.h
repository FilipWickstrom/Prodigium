#pragma once
#include "UsefulHeader.h"
#include "Graphics.h"
#include <keyboard.h>
#include <Mouse.h>
class InputHandler
{
private:
	InputHandler();
	virtual ~InputHandler();
	std::unique_ptr<DirectX::Keyboard> keyboard;
	std::unique_ptr<DirectX::Mouse> mouse;
	int lol;
	static InputHandler* instance;
public:
	DELETE_COPY_ASSIGNMENT(InputHandler);
	static bool Initialize(const HWND& windowHandle);
	static LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

