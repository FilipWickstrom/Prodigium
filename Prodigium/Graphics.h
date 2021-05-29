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
	ID3D11RenderTargetView* backBufferView;
	ID3D11DepthStencilView* depthView;		//Not in use for now
	ID3D11DepthStencilState* zBufferOn;		//Not in use for now
	ID3D11DepthStencilState* zBufferOff;	//Not in use for now
	ID3D11RasterizerState* rasterState;		//Not in use for now
	D3D11_VIEWPORT viewport;
	UINT windowWidth;
	UINT windowHeight;
	float deltaTime;

	bool CreateDeviceAndSwapChain(const HWND& windowHandler, const UINT& windowWidth, const UINT& windowHeight);
	bool CreateZBufferStates();
	void CreateViewPort();
	bool CreateBackBuffer();

public:
	DELETE_COPY_ASSIGNMENT(Graphics);

	static ID3D11Device*& GetDevice();
	static ID3D11DeviceContext* GetContext();
	static IDXGISwapChain*& GetSwapChain();
	static const UINT& GetWindowWidth();
	static const UINT& GetWindowHeight();
	static bool Initialize(const HWND& windowHandler, const UINT& windowWidth, const UINT& windowHeight);
	static void Destroy();
	static void EnableZBuffer();
	static void DisableZBuffer();
	static void SetMainWindowViewport();
	static void BindBackBuffer();
	static void BindBackBuffer(ID3D11DepthStencilView*& depthStencilView);
	static void UnbindBackBuffer();
	static void ClearDisplay();
	static bool SetupGraphics();
	static void SetDeltaTime(const float& time);
	static const float& GetDeltaTime();
};