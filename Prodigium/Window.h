#pragma once
#include <Windows.h>
#include <iostream>
#include "Resource.h"

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
};