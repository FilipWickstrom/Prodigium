#pragma once
#include <d3d11.h>
#include <Windows.h>
#include <iostream>
#include "UsefulHeader.h"

// This class is a wrapper used to create resources for the engine

class Graphics
{
private:
	Graphics();
	virtual ~Graphics();

	static Graphics* instance;
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11DepthStencilState* zBufferOn;
	ID3D11DepthStencilState* zBufferOff;
	bool CreateDeviceAndSwapChain(const HWND& windowHandler, const UINT& windowWidth, const UINT& windowHeight);
	bool CreateZBufferStates();

public:
	DELETE_COPY_ASSIGNMENT(Graphics);

	static ID3D11Device*& GetDevice();
	static ID3D11DeviceContext* GetContext();
	static IDXGISwapChain*& GetSwapChain();
	static bool Initialize(const HWND& windowHandler, const UINT& windowWidth, const UINT& windowHeight);
	static void Destroy();
	static void EnableZBuffer();
	static void DisableZBuffer();
};