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

void Engine::ClearDisplay()
{
	Graphics::ClearDisplay();
}

void Engine::Render()
{
	this->gPass.Prepare();
	this->sceneHandler.Render();
	this->gPass.Clear();
	Graphics::BindBackBuffer();
	this->lightPass.Prepare();
	this->lightPass.Clear();
	Graphics::UnbindBackBuffer();

	//this->testMeshObj.Render();		//DELETE LATER***
	//this->testMeshObj2.Render();	//DELETE LATER***


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
	
	//if (!this->testMeshObj.Initialize("mask_OBJ.obj", "mask_albedo.png"))
	//{
	//	std::cout << "Object1 failed" << std::endl;
	//	return false;
	//}

	//if (!this->testMeshObj2.Initialize("mask_OBJ.obj", "mask_albedo.png", "", {5.0f,0.0f,0.0f}))
	//{
	//	std::cout << "Object2 failed" << std::endl;
	//	return false;
	//}

	return true;
}