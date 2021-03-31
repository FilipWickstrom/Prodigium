#pragma once
#include <d3d11.h>
#include <DirectXMath.h>

class Game
{
private:

	bool isRunning;
	ID3D11Device* device;
	ID3D11DeviceContext* context; // Device Context

public:

	Game();
	~Game();

	void run();

};