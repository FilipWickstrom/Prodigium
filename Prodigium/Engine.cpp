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

	#ifdef _DEBUG
		OpenConsole();
	#endif 


}

Engine::~Engine()
{
	ResourceManager::Destroy();
#ifdef _DEBUG
	DebugInfo::Destroy();
#endif
	Graphics::Destroy();
	this->guiHandler.Shutdown();
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
	this->gPass.ClearScreen();
	this->gPass.Prepare();
	this->sceneHandler.Render();
	this->gPass.Clear();

	// Shadow pass
	this->gPass.Prepare();
	this->sceneHandler.RenderShadows();
	this->gPass.Clear();

	//TODO: Add Fog Pass

	//Bind only 1 render target, backbuffer
	Graphics::BindBackBuffer();
	this->sceneHandler.RenderLights();
	this->lightPass.Prepare();
	this->lightPass.Clear();

	Graphics::BindBackBuffer(this->gPass.GetDepthStencilView());
#ifdef _DEBUG
	DebugInfo::Prepare();
	this->sceneHandler.RenderBoundingBoxes();
	DebugInfo::Clear();
#endif

	// Particle pass
	this->sceneHandler.RenderParticles();

	//Render the skybox on the places where there is no objects visible from depthstencil
	this->skyboxPass.Prepare();
	this->skyboxPass.Clear();

	//Render the blur depending on sanity
	//1.0f is full sanity = no blur
	//0.0f is no sanitiy = max blur
	this->blurPass.Render(this->playerSanity);//REMOVE LATER: JUST FOR TESTING BLUR*** 

	Graphics::BindBackBuffer();
	this->guiHandler.SetPlayerPos(this->playerPos);
	this->guiHandler.Render();

	Graphics::GetSwapChain()->Present(0, 0);
	Graphics::UnbindBackBuffer();
}

void Engine::OpenConsole()
{
	this->RedirectIoToConsole();
}

void Engine::ChangeActiveTrap()
{
	guiHandler.ChangeActiveTrap();
	this->playerSanity -= 0.2f;//REMOVE LATER: JUST FOR TESTING BLUR*** 
}

void Engine::SetPlayerPos(const DirectX::SimpleMath::Vector3& PlayerPos)
{
	this->playerPos = PlayerPos;
}

void Engine::PauseGame()
{
	this->guiHandler.PauseGame();
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

	if (!Graphics::SetupGraphics())
	{
		return false;
	}

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

	this->guiHandler.Initialize(window.GetWindowHandler());
	
	//Max blur radius is 5 for now
	if (!this->blurPass.Initialize(5))
	{
		return false;
	}
	this->playerSanity = 1.0f;//REMOVE LATER: JUST FOR TESTING BLUR*** 

	return true;
}