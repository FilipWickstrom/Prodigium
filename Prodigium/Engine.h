#pragma once
#include "Window.h"
#include <d3d11.h>
#include <fcntl.h>
#include <io.h>
#include "ResourceManager.h"
#include "RenderPass.h"

class Engine
{
private:
	ID3D11RasterizerState* rasterState;
	D3D11_VIEWPORT viewPort;
	Window window;
	GeometryPass gPass;
	LightPass lightPass;

	ID3D11RenderTargetView* backBufferView;
	ID3D11DepthStencilView* depthView;

private:
	bool SetupBackBuffer();
	void SetupViewPort();
	bool StartUp(HINSTANCE& instance, const UINT& width, const UINT& height);
    void RedirectIoToConsole();

public:
	Engine(HINSTANCE& instance, UINT width, UINT height);
	virtual ~Engine();
	DELETE_COPY_ASSIGNMENT(Engine)

	void ClearDisplay();
	void Render();

	virtual bool OnFrame(const float& deltaTime) = 0;
	virtual bool OnStart() = 0;
};