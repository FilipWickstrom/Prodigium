#pragma once
#include <d3d11.h>
#include <Windows.h>
#define BUFFERS 1

/*
	#include should be inside .cpp file for better start up perfomance?
*/

class Game
{
private:

	/*
		General Window and Game start up values.
	*/

	// Boolean for if the game is paused or not.
	bool isRunning;
	double frameTime;

	ID3D11Device* device;
	ID3D11DeviceContext* context;
	IDXGISwapChain* swapChain;
	ID3D11RasterizerState* rasterState;
	D3D11_VIEWPORT viewPort;

	/*
		Window Properties
	*/

	int windowWidth;
	int windowHeight;

	/*
		Setups
	*/

	bool SetupDevice(int width, int height, HWND window);
	void SetupViewPort();
	bool SetupBackBuffer();

public:

	Game();
	~Game();

	// Main loop of the game.
	void run();

	// Start up for device. Run only once outside of loop.
	bool StartUp(int width, int height, HWND window);

private:

	/*
		Deferred rendering
	*/
	ID3D11Texture2D* textures[BUFFERS];
	ID3D11RenderTargetView* renderTargetViews[BUFFERS];
	ID3D11DepthStencilView* depthView;
	ID3D11RenderTargetView* backBufferView;

	/*
		Functions
	*/

	// Clear the space of Backbuffer view.
	void ClearDisplay();
};