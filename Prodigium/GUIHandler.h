#pragma once

#include "UsefulHeader.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Graphics.h"
#include "Window.h"

class GUIHandler
{
private:
	ImGuiIO io;
public:
	GUIHandler();
	virtual ~GUIHandler();
	DELETE_COPY_ASSIGNMENT(GUIHandler);
	void Initialize(const HWND& window);
	void Render();
	void Shutdown();
	void CreateFPSCounter(const float& fps);
};