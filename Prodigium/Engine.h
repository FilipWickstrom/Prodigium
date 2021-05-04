#pragma once
#include "Window.h"
#include <d3d11.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include "ResourceManager.h"
#include "RenderPass.h"
#include "SkyboxPass.h"
#include "InputHandler.h"
#include "SceneHandler.h"
#include "CameraObject.h"

class Engine
{
private:
	Window window;
	GeometryPass gPass;
	LightPass lightPass;
	SkyboxPass skyboxPass;
	bool consoleOpen;
	SceneHandler sceneHandler;

private:
	bool StartUp(const HINSTANCE& instance, const UINT& width, const UINT& height);
	void RedirectIoToConsole();

public:
	DELETE_COPY_ASSIGNMENT(Engine)

	Engine(const HINSTANCE& instance, const UINT& width, const UINT& height);
	virtual ~Engine();

	SceneHandler* SceneHandle();
	void ClearDisplay();
	void Render();
	void OpenConsole();
	virtual bool OnFrame(const float& deltaTime) = 0;
	virtual bool OnStart() = 0;
};