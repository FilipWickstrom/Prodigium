#include "Engine.h"
#include <omp.h>

Engine::Engine(const HINSTANCE& instance, const UINT& width, const UINT& height, Enemy* enemy)
{
	srand((unsigned int)time(NULL));
	this->consoleOpen = false;
	this->playerHp = 100;
	this->cluesCollected = 0;
	this->stopcompl_timer = 0;
	this->slowdown_timer = 0;
#ifdef _DEBUG
	OpenConsole();
#endif 
	if (!this->StartUp(instance, width, height, enemy))
	{
		std::cout << "Failed to initialize Engine!" << std::endl;
		exit(-1);
	}
}

Engine::~Engine()
{
	ResourceManager::Destroy();
#ifdef _DEBUG
	DebugInfo::Destroy();
#endif
	Graphics::Destroy();
	GUIHandler::Shutdown();
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
	std::unordered_map<std::uintptr_t, MeshObject*>* toRender = &this->sceneHandler.EditScene().GetAllCullingObjects();
	toRender->clear();
	//Render the scene to the gbuffers - 3 render targets
	this->gPass.ClearScreen();
	this->gPass.Prepare();
	if (!inGame)
	{
		this->sceneHandler.Render();
	}
	else
	{
		ResourceManager::GetCamera("PlayerCam")->GetFrustum()->Drawable(quadTree, *toRender);
		this->sceneHandler.Render(*toRender);
	}
	 
	// Shadow pass
	if (!inGame)
	{
		this->sceneHandler.RenderShadows();
	}
	else
	{
		this->sceneHandler.RenderShadows(*toRender);
	}
	this->gPass.Clear();


	// SSAO PHASE
	this->gPass.BindSSAO();
	this->sceneHandler.EditScene().RenderSSAO();
	this->gPass.Clear();


	//Bind only 1 render target, backbuffer
	Graphics::BindBackBuffer();
	this->sceneHandler.RenderLights();
	this->sceneHandler.EditScene().RenderSSAOLightPass();
	this->lightPass.Prepare();
	this->lightPass.Clear();

	Graphics::BindBackBuffer(this->gPass.GetDepthStencilView());
#ifdef _DEBUG
	if (inGame)
	{
		DebugInfo::Prepare();
		ResourceManager::GetCamera("PlayerCam")->GetFrustum()->Render();
		DebugInfo::Prepare();
		this->sceneHandler.RenderBoundingBoxes(*toRender);

		DebugInfo::Clear();
	}
#endif

	if (this->options.hasParticles)
	{
		// Particle pass
		this->sceneHandler.RenderParticles();
	}

	//Render the skybox on the places where there is no objects visible from depthstencil
	this->skyboxPass.Prepare();
	this->skyboxPass.Clear();

	if (!this->isPaused && this->options.hasBlur && this->playerSanity != 1.0f)
	{
		//Render the blur depending on sanity
		//1.0f is full sanity = no blur
		//0.0f is no sanitiy = max blur
		this->blurPass.Render(this->playerSanity);
	}

	if (this->isPaused)
	{
		this->blurPass.Render(0);
	}

	Graphics::BindBackBuffer();
	Graphics::SetMainWindowViewport();
	GUIHandler::Render(this->playerHp, this->cluesCollected, this->stopcompl_timer, this->slowdown_timer, this->options);

	Graphics::GetSwapChain()->Present(0, 0);
	Graphics::UnbindBackBuffer();
}

void Engine::Update(const float& deltaTime)
{
	// So we don't go over a certain value
	this->playerHp = std::min(this->playerHp, 100);
	this->cluesCollected = std::min(this->cluesCollected, (this->options.difficulty * 2));

	// Update the sanity depending on the health.
	this->playerSanity = this->playerHp * 0.01f;

	if (this->slowdown_timer > 0.0f)
	{
		this->slowdown_timer -= 1.0f * deltaTime;
		this->slowdown_timer = std::max(this->slowdown_timer, 0.0f);
	}

	if (this->stopcompl_timer > 0.0f)
	{
		this->stopcompl_timer -= 1.0f * deltaTime;
		this->stopcompl_timer = std::max(this->stopcompl_timer, 0.0f);
	}

	
}

void Engine::OpenConsole()
{
	this->RedirectIoToConsole();
}

void Engine::ChangeActiveTrap()
{
	GUIHandler::ChangeActiveTrap();
}

bool Engine::StartUp(const HINSTANCE& instance, const UINT& width, const UINT& height, Enemy* enemy)
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

	if (!GUIHandler::Initialize(window.GetWindowHandler()))
	{
		return false;
	}

	//Max blur radius is 5 for now
	if (!this->blurPass.Initialize(5))
	{
		return false;
	}
	AIHandler::Initialize();

	AIHandler::CreateNodes();
	this->playerSanity = 1.0f;//REMOVE LATER: JUST FOR TESTING BLUR*** 

	return true;
}