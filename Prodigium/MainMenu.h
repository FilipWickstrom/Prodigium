#pragma once
#include "Graphics.h"
#include <SimpleMath.h>
class MainMenu
{
private:

	bool inMenu;
	ID3D11Buffer* viewBuffer;
	ID3D11Buffer* eyeBuffer;

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
	void ZoomIn(DirectX::SimpleMath::Vector3 endPos);

	void Update();
};