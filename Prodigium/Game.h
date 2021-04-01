#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <iostream>

class Game
{
private:

	bool isRunning;
	ID3D11Device* device;
	ID3D11DeviceContext* context; // Device Context

public:

	Game();
	~Game();

	void run();

	/*
	* Setups
	*/

	//bool SetupWindow(HINSTANCE instance, UINT width, UINT height, int nCmdShow, HWND& window);

};