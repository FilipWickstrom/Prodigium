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
	ImGuiIO io;
	bool trap1Active, trap2Active;
	int imageWidth, imageHeight;
	DirectX::SimpleMath::Vector3 playerPos;
	ID3D11ShaderResourceView* textureTrap1;
	ID3D11ShaderResourceView* textureTrap2;
	ID3D11ShaderResourceView* textureBrain;
	ID3D11ShaderResourceView* textureOutline;
	void CreateDebugGUI();
	void CreateTrapGUI();
	void CreateBrainGUI();
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
public:
	GUIHandler();
	virtual ~GUIHandler();
	DELETE_COPY_ASSIGNMENT(GUIHandler);
	void Initialize(const HWND& window);
	void Render();
	void Shutdown();
	void ChangeActiveTrap();
	void setPlayerPos(const DirectX::SimpleMath::Vector3& playerPos);

	// Returns the trap1Active bool.
	const bool ActiveTrap() const;
};