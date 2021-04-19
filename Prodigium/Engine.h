#pragma once
#include "Window.h"
#include <d3d11.h>
#include "GeometryPass.h"
#include "LightPass.h"
#include <fcntl.h>
#include <io.h>
#include "ResourceManager.h"


#include "MeshObject.h"	//testing****

class Engine
{
private:
	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11RasterizerState* rasterState;
	D3D11_VIEWPORT viewPort;
	Window window;
	GeometryPass gPass;
	LightPass lightPass;

	ID3D11RenderTargetView* backBufferView;
	ID3D11DepthStencilView* depthView;

	MeshObject meshtest;	//testing****

private:
	bool SetupDevice();
	bool SetupBackBuffer();
	void SetupViewPort();

public:
	Engine();
	virtual ~Engine();
	DELETE_COPY_ASSIGNMENT(Engine)

    void RedirectIoToConsole();
	bool StartUp(HINSTANCE& instance, const UINT& width, const UINT& height);
	void ClearDisplay();
	void PresentScene();
};