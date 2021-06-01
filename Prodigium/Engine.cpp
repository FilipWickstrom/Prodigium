#include "Engine.h"
#include <omp.h>

Engine::Engine(const HINSTANCE& instance, const UINT& width, const UINT& height)
{
	srand((unsigned int)time(NULL));
	this->consoleOpen = false;
	this->stopcompl_timer = 0;
	this->slowdown_timer = 0;
#ifdef _DEBUG
	OpenConsole();
#endif 
	if (!this->StartUp(instance, width, height))
	{
		std::cout << "Failed to initialize Engine!" << std::endl;
		exit(-1);
	}
}

Engine::~Engine()
{
	this->Shutdown();
#ifdef _DEBUG
	DebugInfo::Destroy();
#endif
	this->gPass.Destroy();
	this->lightPass.Destroy();
	this->skyboxPass.Destroy();
	this->blurPass.Destroy();
	Graphics::Destroy();
	GUIHandler::Destroy();
	InputHandler::Destroy();
	ResourceManager::Destroy();
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
	BlurLevel current = this->blurPass.GetBlurLevel();
	this->blurPass.SetBlurLevel(BlurLevel::HIGH);
	this->blurPass.Render(&this->sceneHandler.EditScene().GetSSAOAccessView());
	this->blurPass.SetBlurLevel(current);

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
	if (this->options.hasBlur)
	{
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

void Engine::Shutdown()
{
	Graphics::GetContext()->PSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->GSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->CSSetShader(NULL, NULL, NULL);

	ID3D11RenderTargetView* cleanTargets[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT] = { nullptr };
	ID3D11ShaderResourceView* cleanShader = { nullptr };
	ID3D11Buffer* cleanBuffer = { nullptr };
	ID3D11VertexShader* vertexNull = nullptr;
	ID3D11PixelShader* pixelNull = nullptr;
	ID3D11GeometryShader* geoNull = nullptr;
	ID3D11InputLayout* inputNull = nullptr;
	ID3D11ComputeShader* computeNull = nullptr;
	ID3D11UnorderedAccessView* unacNull = nullptr;
	Graphics::GetContext()->OMSetRenderTargets(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT, cleanTargets, nullptr);
	for (int i = 0; i < 8; i++)
	{
		Graphics::GetContext()->PSSetShaderResources(i, 1, &cleanShader);
		Graphics::GetContext()->VSSetShaderResources(i, 1, &cleanShader);
		Graphics::GetContext()->VSSetConstantBuffers(i, 1, &cleanBuffer);
		Graphics::GetContext()->GSSetConstantBuffers(i, 1, &cleanBuffer);
		Graphics::GetContext()->PSSetConstantBuffers(i, 1, &cleanBuffer);
		Graphics::GetContext()->CSSetConstantBuffers(i, 1, &cleanBuffer);
		Graphics::GetContext()->CSSetUnorderedAccessViews(i, 1, &unacNull, nullptr);
	}
	Graphics::GetContext()->GSSetShader(geoNull, nullptr, 0);
	Graphics::GetContext()->CSSetShader(computeNull, nullptr, 0);
	Graphics::GetContext()->HSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->DSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->PSSetShader(pixelNull, nullptr, 0);
	Graphics::GetContext()->VSSetShader(vertexNull, nullptr, 0);
	Graphics::GetContext()->IASetInputLayout(inputNull);
}

void Engine::Blur(const BlurLevel& amount)
{
	this->blurPass.SetBlurLevel(amount);
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