#pragma once
#include "UsefulHeader.h"
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#include "Graphics.h"
#include "Window.h"
#include <stb/stb_image.h>
#include <SimpleMath.h>

class GUIHandler
{
private:
	GUIHandler();
	virtual ~GUIHandler();

	static GUIHandler* instance;

	ImGuiIO io;
	bool trap1Active, trap2Active, isPaused, shouldQuit;
	int imageWidth, imageHeight;
	DirectX::SimpleMath::Vector3 playerPos;
	ID3D11ShaderResourceView* textureTrap1;
	ID3D11ShaderResourceView* textureTrap2;
	ID3D11ShaderResourceView* textureBrain;
	ID3D11ShaderResourceView* textureOutline;
	void CreateDebugGUI();
	void CreateTrapGUI();
	void CreateBrainGUI();
	void CreatePauseMenu();
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
public:
	DELETE_COPY_ASSIGNMENT(GUIHandler);
	static const bool Initialize(const HWND& window);
	static void Render();
	static void Shutdown();
	static void ChangeActiveTrap();
	static void SetPlayerPos(const DirectX::SimpleMath::Vector3& playerPos);
	static void PauseGame();
	static void ResumeGame();
	static const bool ShouldQuit();
	// Returns the trap1Active bool.
	static const bool ActiveTrap();
};