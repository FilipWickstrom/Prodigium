#pragma once
#include "UsefulHeader.h"
#include "Graphics.h"
#include <keyboard.h>
class InputHandler
{
private:
	InputHandler();
	virtual ~InputHandler();
	static InputHandler* instance;
public:
	DELETE_COPY_ASSIGNMENT(InputHandler);
	static LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};

