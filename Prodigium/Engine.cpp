#include "Engine.h"

Engine::Engine()
{
	this->backBufferView = nullptr;
	this->depthView = nullptr;
	this->rasterState = nullptr;
	this->viewPort = {};
}

Engine::~Engine()
{
	if (this->depthView)
		this->depthView->Release();
	if (this->rasterState)
		this->rasterState->Release();
	if (this->backBufferView)
		this->backBufferView->Release();
	Graphics::Destroy();
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

	Graphics::GetContext()->ClearRenderTargetView(this->backBufferView, color);
}

void Engine::PresentScene()
{
	this->gPass.RenderGPass(Graphics::GetContext());
	Graphics::GetContext()->RSSetViewports(1, &viewPort);
	ID3D11RenderTargetView* clearRenderTargets[BUFFER_COUNT] = { nullptr };
	Graphics::GetContext()->OMSetRenderTargets(BUFFER_COUNT, clearRenderTargets, nullptr);
	Graphics::GetContext()->OMSetRenderTargets(1, &backBufferView, depthView);

	this->testMeshObj.Render();		//DELETE LATER***
	this->testMeshObj2.Render();	//DELETE LATER***

	this->lightPass.Render(Graphics::GetContext());

	Graphics::GetSwapChain()->Present(0, 0);
}

bool Engine::StartUp(HINSTANCE& instance, const UINT& width, const UINT& height)
{

	if (!this->window.SetupWindow(instance, width, height))
	{
		return false;
	}

	if (!Graphics::Initialize(this->window.GetWindowHandler(), width, height))
	{
		return false;
	}

	if (!this->SetupBackBuffer())
	{
		return false;
	}

	ResourceManager::Initialize();

	this->SetupViewPort();

	if (!this->gPass.Initialize(Graphics::GetDevice(), width, height))
	{
		return false;
	}

	if (!this->lightPass.Initialize(Graphics::GetDevice(), width, height))
	{
		return false;
	}

	
	if (!this->testMeshObj.Initialize("mask_OBJ.obj", "mask_albedo.png"))
	{
		std::cout << "Object1 failed" << std::endl;
		return false;
	}

	if (!this->testMeshObj2.Initialize("mask_OBJ.obj", "mask_albedo.png", "", {5.0f,0.0f,0.0f}))
	{
		std::cout << "Object2 failed" << std::endl;
		return false;
	}

	return true;
}

bool Engine::SetupBackBuffer()
{
	HRESULT hr;

	ID3D11Texture2D* tempTexture = nullptr;
	if (FAILED(Graphics::GetSwapChain()->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&tempTexture))))
	{
		return false;
	}

	hr = Graphics::GetDevice()->CreateRenderTargetView(tempTexture, 0, &this->backBufferView);
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