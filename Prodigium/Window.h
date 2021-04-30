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
	bool SetupWindow(const HINSTANCE& instance, const UINT& width, const UINT& height);
	const HWND& GetWindowHandler()const;
	const UINT& GetWindowWidth()const;
	const UINT& GetWindowHeight()const;
};