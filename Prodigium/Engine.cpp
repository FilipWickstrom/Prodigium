#include "Engine.h"

Engine::Engine()
{
	this->backBufferView = nullptr;
	this->context = nullptr;
	this->depthView = nullptr;
	this->device = nullptr;
	this->rasterState = nullptr;
	this->swapChain = nullptr;
	this->viewPort = {};
}

Engine::~Engine()
{
	if (this->depthView)
		this->depthView->Release();
	if (this->rasterState)
		this->rasterState->Release();
	if (this->swapChain)
		this->swapChain->Release();
	if (this->context)
		this->context->Release();
	if (this->device)
		this->device->Release();
	if (this->backBufferView)
		this->backBufferView->Release();
}

void Engine::RedirectIoToConsole()
{
	AllocConsole();
	HANDLE stdHandle;
	int hConsole;
	FILE* fp;
	stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
	fp = _fdopen(hConsole, "w");

	freopen_s(&fp, "CONOUT$", "w", stdout);
}

void Engine::ClearDisplay()
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

void Engine::PresentScene()
{
	this->gPass.RenderGPass(context);
	this->context->RSSetViewports(1, &viewPort);
	
	this->meshtest.Render(this->context);

	ID3D11RenderTargetView* clearRenderTargets[BUFFER_COUNT] = { nullptr };
	this->context->OMSetRenderTargets(BUFFER_COUNT, clearRenderTargets, nullptr);
	this->context->OMSetRenderTargets(1, &backBufferView, depthView);
	this->lightPass.Render(context);

	this->swapChain->Present(0, 0);
}

bool Engine::StartUp(HINSTANCE& instance, const UINT& width, const UINT& height)
{
	if (!this->window.SetupWindow(instance, width, height))
	{
		return false;
	}

	if (!this->SetupDevice())
	{
		return false;
	}

	if (!this->SetupBackBuffer())
	{
		return false;
	}

	this->SetupViewPort();

	if (!this->gPass.Initialize(device, width, height))
	{
		return false;
	}

	if (!this->lightPass.Initialize(device, width, height))
	{
		return false;
	}

	meshtest.Initialize(this->device, "Necklace/necklace_FBX.fbx");				//testing****
	meshtest.BuildMatrix(this->device, { 0, 0, 5 }, { 1, 1, 1 }, { 0,0,0 });	//testing***

	return true;
}

bool Engine::SetupDevice()
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
	swapDesc.BufferDesc.Width = this->window.GetWindowWidth();
	swapDesc.BufferDesc.Height = this->window.GetWindowHeight();

	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;

	// One front buffer and one Back
	swapDesc.BufferCount = 2;

	// Allow our backbuffer to be manipulated by any compute shader, e.g. allows simple blur effect.
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	swapDesc.Windowed = true;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapDesc.Flags = 0;
	swapDesc.OutputWindow = this->window.GetWindowHandler();

	hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, flags, directXfeature, 1, D3D11_SDK_VERSION,
		&swapDesc, &this->swapChain, &this->device, nullptr, &this->context);

	return !FAILED(hr);
}

bool Engine::SetupBackBuffer()
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

void Engine::SetupViewPort()
{
	this->viewPort.Width = (float)window.GetWindowWidth();
	this->viewPort.Height = (float)window.GetWindowHeight();
	this->viewPort.TopLeftX = 0.f;
	this->viewPort.TopLeftY = 0.f;
	this->viewPort.MinDepth = 0.f;
	this->viewPort.MaxDepth = 1.0f;
}