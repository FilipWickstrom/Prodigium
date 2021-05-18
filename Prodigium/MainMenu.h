#pragma once
#include "Graphics.h"
#include <SimpleMath.h>
#include "imgui.h"
class MainMenu
{
private:

	struct Package
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix projection;
	};
	struct FogBuffer
	{
		DirectX::SimpleMath::Matrix viewMatrix;
		DirectX::SimpleMath::Vector4 cameraPos;
		DirectX::SimpleMath::Vector4 fogColour;
		float fogStart;
		float fogRange;
		DirectX::SimpleMath::Vector2 padding;
	};
	bool inMenu;
	ID3D11Buffer* viewBuffer;
	ID3D11Buffer* eyeBuffer;
	float distToGoal;
	float time;

private:

	DirectX::SimpleMath::Matrix menuView;
	DirectX::SimpleMath::Matrix menuProj;
	DirectX::SimpleMath::Vector4 eyePos;

	const bool SetUpBuffer();

public:

	MainMenu();
	virtual ~MainMenu();
	const bool IsInMenu() const;
	void Init();

	// Switch between menu and game.
	void Switch(bool boolean = false);

	// Perform the zoom in.
	void ZoomIn(DirectX::SimpleMath::Vector4 endPos, const float& deltaTime, bool& inGoal);
	void Reset();

	void Update();
};