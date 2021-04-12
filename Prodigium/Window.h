#pragma once
#include <Windows.h>
#include <iostream>

class Window
{
private:
	HWND windowHandler;
	UINT WIDTH;
	UINT HEIGHT;

public:
	Window();
	~Window();
	bool SetupWindow(HINSTANCE& instance, UINT width, UINT height);
	const HWND& GetWindowHandler()const;
	const UINT& GetWindowWidth()const;
	const UINT& GetWindowHeight()const;
	static LRESULT WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
};