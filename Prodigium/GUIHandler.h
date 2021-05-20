#pragma once
#include "UsefulHeader.h"
#pragma warning(push, 0)
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
#pragma warning(pop)
#include "Graphics.h"
#include "Window.h"
#include <stb/stb_image.h>
#include <SimpleMath.h>

#define CLUES 4

class GUIHandler
{
private:
	GUIHandler();
	virtual ~GUIHandler();

	static GUIHandler* instance;

	ImGuiIO io;
	bool trap1Active, trap2Active, isPaused, shouldQuit, shouldResume, showMainMenu, showGameGUI;
	int imageWidth, imageHeight;
	DirectX::SimpleMath::Vector3 playerPos;
	ID3D11ShaderResourceView* textureTrap1;
	ID3D11ShaderResourceView* textureTrap2;
	ID3D11ShaderResourceView* textureBrain;
	ID3D11ShaderResourceView* textureOutline;
	void RenderDebugGUI();
	void RenderTrapGUI(float& timer1, float& timer2);
	void RenderBrainGUI(int playerHp, int clues);

	void RenderPauseMenu();
	void RenderMainMenu();

	void QuitGame();
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
public:
	DELETE_COPY_ASSIGNMENT(GUIHandler);
	static const bool Initialize(const HWND& window);
	static void Render(int playerHp, int clues, float& timer1, float& timer2);
	static void Shutdown();
	static void ChangeActiveTrap();
	static void SetPlayerPos(const DirectX::SimpleMath::Vector3& playerPos);
	static void PauseGame();
	static void ResumeGame();
	static const bool ShouldResume();
	static const bool ShouldQuit();
	static void ShowMainMenu(const bool& show);
	static void ShowGameGUI(const bool& show);
	// Returns the trap1Active bool.
	static const bool ActiveTrap();
};