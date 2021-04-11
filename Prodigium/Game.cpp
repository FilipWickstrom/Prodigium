#include "Game.h"
#include <iostream>
#include <omp.h>

bool Game::SetupDevice(int width, int height, HWND window)
{
	HRESULT hr;

	// Debug mode
	UINT flags = 0;
	if (_DEBUG)
		flags = D3D11_CREATE_DEVICE_DEBUG;

	// Change this feature level to change DirectX version, Most stable is DirectX11
	D3D_FEATURE_LEVEL directXfeature[] = { D3D_FEATURE_LEVEL_11_0 };
	DXGI_SWAP_CHAIN_DESC swapDesc;

	/*
		What refresh rate do we lock to?
		Numerator - 0 and Denominator - 1 means unlocked framerate.

		for locked to 60 fps Numerator should be 60.
	*/
	swapDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.Height = height;


	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	// One front buffer and one Back
	swapDesc.BufferCount = 2;

	// Allow our backbuffer to be manipulated by any compute shader, e.g. allows simple blur effect.
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = 0;
	swapDesc.OutputWindow = window;

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, directXfeature, 1, D3D11_SDK_VERSION,
		&swapDesc, &this->swapChain, &this->device, nullptr, &this->context);

	return !FAILED(hr);
}

void Game::SetupViewPort()
{
	this->viewPort.Width = (float)this->windowWidth;
	this->viewPort.Height = (float)this->windowHeight;
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;
	this->viewPort.TopLeftX = 0.0f;
	this->viewPort.TopLeftY = 0.0f;
}

bool Game::SetupBackBuffer()
{
	HRESULT hr;

	ID3D11Texture2D* tempTexture = nullptr;
	if (FAILED(this->swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tempTexture))))
	{
		return false;
	}

	hr = this->device->CreateRenderTargetView(tempTexture, 0, &this->backBufferView);
	tempTexture->Release();
	return !FAILED(hr);
}

void Game::ClearDisplay()
{
	float color[4];

	// Red
	color[0] = 0.25;

	// Green
	color[1] = 0.25;

	// Blue
	color[2] = 1;

	// Alpha
	color[3] = 0.75;

	this->context->ClearRenderTargetView(this->backBufferView, color);
}

Game::Game()
{
	/*
		Just a bunch of crap, setting everything to start value to lower amount of  warnings.
	*/
	this->isRunning = true;
	this->device = 0;
	this->context = 0;
	this->swapChain = 0;
	this->rasterState = 0;
	this->backBufferView = 0;
	this->depthView = 0;
	this->frameTime = 0;

	this->windowHeight = 0;
	this->windowWidth = 0;

	for (int i = 0; i < BUFFERS; i++)
	{
		this->renderTargetViews[i] = 0;
		this->textures[i] = 0;
	}
}

Game::~Game()
{
	// Gotta get rid of the not very tasty memory leeks :)
	if (this->device)
		this->device->Release();

	if (this->context)
		this->context->Release();

	if (this->swapChain)
		this->swapChain->Release();

	if (this->backBufferView)
		this->backBufferView->Release();
}

void Game::run()
{
	// Calculate difference since last run() call.
	double frameDiff = omp_get_wtime() - this->frameTime;

	// Active game loop.
	if (isRunning)
	{
		this->ClearDisplay();

		this->swapChain->Present(0, 0);
	}

	// Retake the frame time for difference calculation later on.
	this->frameTime = omp_get_wtime();
}

bool Game::StartUp(int width, int height, HWND window)
{
	// Save for reference in other functions.
	this->windowHeight = height;
	this->windowWidth = width;

	// Start taking one frame second on start up.
	this->frameTime = omp_get_wtime();

	if (!this->SetupDevice(width, height, window))
	{
		return false;
	}

	if (!this->SetupBackBuffer())
	{
		return false;
	}

	this->SetupViewPort();

	return true;
}
