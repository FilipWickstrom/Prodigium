#include "Engine.h"
#include <omp.h>

Engine::Engine(const HINSTANCE& instance, const UINT& width, const UINT& height, Enemy* enemy)
{
	srand((unsigned int)time(NULL));
	this->consoleOpen = false;
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
	this->gPass.Destroy();
	this->lightPass.Destroy();
	this->skyboxPass.Destroy();
	Graphics::Destroy();
	GUIHandler::Destroy();
	InputHandler::Destroy();
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

SceneHandler* Engine::SceneHandler()
{
	return &sceneHandler;
}

void Engine::ClearDisplay()
{
	Graphics::ClearDisplay();
}

void Engine::Render(Player* player)
{
	this->sceneHandler.EditScene().GetAllStaticObjects().clear();

	//Render the scene to the gbuffers - 3 render targets
	this->gPass.ClearScreen();
	this->gPass.Prepare();
	if (!inGame)
	{
		this->sceneHandler.Render();
	}
	else
	{
		ResourceManager::GetCamera("PlayerCam")->GetFrustum()->Drawable(quadTree, this->sceneHandler.EditScene().GetAllStaticObjects());
		this->sceneHandler.RenderAllObjects();
	}

	// Shadow pass
	if (!inGame)
	{
		this->sceneHandler.RenderShadows();
	}
	else
	{
		this->sceneHandler.RenderAllShadows();
	}
	this->gPass.Clear();

	// SSAO PHASE
	this->gPass.BindSSAO();
	this->sceneHandler.EditScene().RenderSSAO();
	this->gPass.Clear();
	this->blurPass.SetBlurLevel(BlurLevel::MEDIUM);
	this->blurPass.Render(&this->sceneHandler.EditScene().GetSSAOAccessView());

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
		//DebugInfo::Prepare();
		//ResourceManager::GetCamera("PlayerCam")->GetFrustum()->Render();
		DebugInfo::Prepare();
		this->sceneHandler.RenderAllBoundingBoxes();

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

	//Do blur on screen if it is on and player exists
	if (this->options.hasBlur && player)
	{
		//Game is paused - then we use a high blur
		if (this->isPaused)
		{
			//More effective to change sigma to higher than adding more in radius
			this->blurPass.SetBlurLevel(BlurLevel::HELLAHIGH);
		}
		else
		{
			this->blurPass.SetBlurLevel(player->GetBlurLevel());
		}
		this->blurPass.Render();
	}

	Graphics::BindBackBuffer();
	Graphics::SetMainWindowViewport();
	if (player)
	{
		GUIHandler::Render(player->GetSanity(), player->GetCollectedClues(), this->stopcompl_timer, this->slowdown_timer, this->options);
	}
	else
	{
		GUIHandler::Render(0, 0, this->stopcompl_timer, this->slowdown_timer, this->options);
	}

	Graphics::GetSwapChain()->Present(0, 0);
	Graphics::UnbindBackBuffer();
}

void Engine::OpenConsole()
{
	this->RedirectIoToConsole();
}

void Engine::ToggleSSAO(bool toggle)
{
	this->lightPass.ToggleSSAO(toggle);
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

	if (!this->blurPass.Initialize())
	{
		return false;
	}

	return true;
}