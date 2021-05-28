#pragma once
#include <d3d11.h>
#include <fstream>
#include <vector>
#include "Graphics.h"

const double PI = 3.14159265359;
const UINT MAXWEIGHT = 8;
const UINT MAXRADIUS = MAXWEIGHT - 1;	//Bad for performance
const UINT MINRADIUS = 1;

/*
Guassian blur filter that makes everything
blurry on the screen, but still so that you 
can see objects better than flat box blur.

The filter is rendered in two passess
like a cross instead of like a square. 
Much faster and saves alot of memory.

Last update:
	* More memory effective blur: 
		- less weights as constant buffer
		- from 112 bytes to 48 bytes in cb
*/

enum class BlurState
{
	NOBLUR, RAD1, 
	RAD2, RAD3, 
	RAD4, RAD5, 
	RAD6, RAD7
};

class BlurFilter
{
private:
	ID3D11ComputeShader* computeShader;
	ID3D11UnorderedAccessView* unorderedAccessView;		//Backbuffer
	ID3D11Buffer* settingsBuffer;

	//CBuffer for the compute shader
	struct BlurSettings
	{
		UINT blurRadius;
		bool useVerticalBlur;
		float padding[2];
		float weights[MAXWEIGHT];
	} blurSettings;

	BlurState screenBlurState;
	//Array of weights for screenBlur


	//Stuff to render on an other surface
	//BlurState otherState
	//Other array of weights
	//Another unorderedAccessView that can be used for textures

private:
	//Creating directx buffers and shaders
	bool CreateComputeShader();
	bool CreateUnorderedAccessView();
	bool CreateSettingsBuffer();
	
	//Help functions
	void GenerateGaussFilter(UINT radius, float sigma = 0.0f);
	void UpdateBlurSettings();
	void SwapBlurDirection();

public: 
	BlurFilter();
	~BlurFilter();

	bool Initialize();
	void ChangeScreenBlur(BlurState state, float sigma = 0.0f);
	void RenderScreenBlur();

	//void RenderUAV(uav, BlurState);
	void Render(float blurPercentage, ID3D11UnorderedAccessView& textureView);
};