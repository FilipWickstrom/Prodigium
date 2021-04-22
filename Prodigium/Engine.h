#pragma once
#include "Window.h"
#include <d3d11.h>
#include <fcntl.h>
#include <io.h>
#include "ResourceManager.h"
#include "RenderPass.h"

#include "MeshObject.h"		//DELETE LATER***
#include "CameraObject.h"	//DELETE LATER***

class Engine
{
private:
	Window window;
	GeometryPass gPass;
	LightPass lightPass;

	MeshObject testMeshObj;		//DELETE LATER***
	//CameraObject testCamera;	//DELETE LATER***
	//MeshObject testMeshObj2;	//DELETE LATER***

private:
	bool StartUp(HINSTANCE& instance, const UINT& width, const UINT& height);
    void RedirectIoToConsole();

public:
	Engine(HINSTANCE& instance, UINT width, UINT height);
	virtual ~Engine();
	DELETE_COPY_ASSIGNMENT(Engine)

	void ClearDisplay();
	void Render();

	virtual bool OnFrame(const float& deltaTime) = 0;
	virtual bool OnStart() = 0;
};