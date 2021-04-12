#pragma once
#include "Window.h"
#include <d3d11.h>

class Engine
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11RasterizerState* rasterState;
	D3D11_VIEWPORT viewPort;
	Window window;

	/*
		Deferred rendering
	*/
	ID3D11RenderTargetView* backBufferView;
	static const int bufferCount = 1;
	ID3D11Texture2D* textures[bufferCount];
	ID3D11RenderTargetView* renderTargetViews[bufferCount];
	ID3D11ShaderResourceView* shaderResourceViews[bufferCount];
	ID3D11DepthStencilView* depthView;

private:
	bool SetupDevice();
	bool SetupBackBuffer();
	void SetupViewPort();

public:
	Engine();
	virtual ~Engine();
	Engine(const Engine& other) = delete;
	Engine& operator=(const Engine& other) = delete;

	bool StartUp(HINSTANCE& instance, UINT width, UINT height);
	void ClearDisplay();
	void PresentScene();
};