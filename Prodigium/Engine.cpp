#include "Engine.h"

Engine::Engine(const HINSTANCE& instance, const UINT& width, const UINT& height)
{
	srand((unsigned int)time(NULL));
	this->consoleOpen = false;


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
	bool result = false;
	if (!consoleOpen)
	{
		FILE* fp;
		AllocConsole();
		// Redirect STDIN if the console has an input handle
		if (GetStdHandle(STD_INPUT_HANDLE) != INVALID_HANDLE_VALUE)
			if (freopen_s(&fp, "CONIN$", "r", stdin) != 0)
				result = false;
			else
				setvbuf(stdin, NULL, _IONBF, 0);

		// Redirect STDOUT if the console has an output handle
		if (GetStdHandle(STD_OUTPUT_HANDLE) != INVALID_HANDLE_VALUE)
			if (freopen_s(&fp, "CONOUT$", "w", stdout) != 0)
				result = false;
			else
				setvbuf(stdout, NULL, _IONBF, 0);

		// Redirect STDERR if the console has an error handle
		if (GetStdHandle(STD_ERROR_HANDLE) != INVALID_HANDLE_VALUE)
			if (freopen_s(&fp, "CONOUT$", "w", stderr) != 0)
				result = false;
			else
				setvbuf(stderr, NULL, _IONBF, 0);

		// Make C++ standard streams point to console as well.
		std::ios::sync_with_stdio(true);

		// Clear the error state for each of the C++ standard streams.
		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
		//AllocConsole();
		//HANDLE stdHandle;
		//int hConsole;
		//FILE* fp;
		//stdHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		//hConsole = _open_osfhandle((intptr_t)stdHandle, _O_TEXT);
		//fp = _fdopen(hConsole, "w");
		//freopen_s(&fp, "CONOUT$", "w", stdout);
		consoleOpen = true;
	}
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
	this->sceneHandler.Render();

	this->shadower.RenderStatic();
	this->sceneHandler.Render();
	this->shadower.Clean();
	this->gPass.Clear();

	//Bind only 1 render target, backbuffer
	Graphics::BindBackBuffer();
	this->sceneHandler.RenderLights();
	this->shadower.RenderLightPass();
	this->lightPass.Prepare();
	this->lightPass.Clear();

	//Render the skybox on the places where there is no objects visible from depthstencil
	Graphics::BindBackBuffer(this->gPass.GetDepthStencilView());
	this->skyboxPass.Prepare();
	this->skyboxPass.Clear();

	Graphics::GetSwapChain()->Present(0, 0);
	Graphics::UnbindBackBuffer();
}

void Engine::OpenConsole()
{
	this->RedirectIoToConsole();
}

bool Engine::StartUp(const HINSTANCE& instance, const UINT& width, const UINT& height)
{
	if (!InputHandler::Initialize())
	{
		return false;
	}
	if (!this->window.SetupWindow(instance, width, height))
	{
		return false;
	}
	InputHandler::setWindow(window.GetWindowHandler());
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

	LightStruct L;
	L.direction = { 0.f, -500.0f, 250.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 0.0f, 50.0f, 300.0f, 20.0f };
	this->shadower.SetUp(L);

	return true;
}