#include "Engine.h"

Engine::Engine(HINSTANCE& instance, UINT width, UINT height)
{
	this->RedirectIoToConsole();

	if (!this->StartUp(instance, width, height))
	{
		std::cout << "Failed to initialize Engine!" << std::endl;
		exit(-1);
	}

}

Engine::~Engine()
{
	ResourceManager::Destroy();
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

SceneHandler* Engine::SceneHandle()
{
	return &sceneHandler;
}

void Engine::ClearDisplay()
{
	Graphics::ClearDisplay();
}

void Engine::Render()
{
	//Render the scene to the gbuffers - 3 render targets
	this->gPass.Prepare();
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &this->gameCam.GetViewProjMatrix());
	this->sceneHandler.Render();
	this->gPass.Clear();

	//Bind only 1 render target, backbuffer
	Graphics::BindBackBuffer();
	this->lightPass.Prepare();
	this->lightPass.Clear();

	//Render the skybox on the places where there is no objects visible from depthstencil
	Graphics::BindBackBuffer(this->gPass.GetDepthStencilView());
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &this->gameCam.GetViewProjMatrix());
	this->skyboxPass.Prepare();
	this->skyboxPass.Clear();

	Graphics::GetSwapChain()->Present(0, 0);
	Graphics::UnbindBackBuffer();
}

bool Engine::StartUp(HINSTANCE& instance, const UINT& width, const UINT& height)
{
	if (!InputHandler::Initialize(window.GetWindowHandler()))
	{
		return false;
	}
	if (!this->window.SetupWindow(instance, width, height))
	{
		return false;
	}

	if (!Graphics::Initialize(this->window.GetWindowHandler(), width, height))
	{
		return false;
	}
	ResourceManager::Initialize();

	Graphics::SetMainWindowViewport();

	if (!this->gPass.Initialize())
	{
		return false;
	}

	if (!this->lightPass.Initialize())
	{
		return false;
	}

	if (!this->skyboxPass.Initialize())
	{
		return false;
	}

	if (!this->gameCam.Initialize(width, height, 0.1f, 100.0f, XM_PI * 0.5f, { 0.f, 0.f, -5.f }))
	{
		return false;
	}

	this->sceneHandler.EditScene().Add("book_OBJ.obj", "book_albedo.png", "", { 0.0f, 0.0f, 5.0f }, { -XM_PI * 0.5f,0, XM_PI * 0.5f });
	this->sceneHandler.EditScene().Add("book_OBJ.obj", "book_albedo.png", "", { 10.0f, 0.0f, 5.0f }, { -XM_PI * 0.5f,0, XM_PI * 0.5f });

	return true;
}