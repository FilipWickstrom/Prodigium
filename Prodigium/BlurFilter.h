#pragma once
#include <d3d11.h>
#include <fstream>
#include <vector>
#include "Graphics.h"

const double PI = 3.14159265359;
const UINT MAXRADIUS = 15;	//Not good for performance
const UINT MINRADIUS = 1;
const UINT MAXWEIGHTS = 16;

/*
Guassian blur filter that makes everything
blurry on the screen, but still so that you 
can see objects better than flat box blur.

- In Render(float percentage): 0.0f to 1.0f
  which is how much blur to use. 1.0f is maxblur 
  which is going to use maxradius.

  Update:
  * More memory effective blur
  * 
*/

enum class BlurState
{
	NOBLUR = 0,
	MINBLUR = MINRADIUS,
	RAD2, RAD3, RAD4, RAD5,  RAD6,
	RAD7, RAD8, RAD9, RAD10, RAD11,
	RAD12,RAD13,RAD14,
	MAXBLUR = MAXRADIUS
};

class BlurFilter
{
private:
	ID3D11ComputeShader* computeShader;
	ID3D11UnorderedAccessView* unorderedAccessView;		//Backbuffer
	//Another unorderedAccessView that can be used for textures
	ID3D11Buffer* settingsBuffer;

	//CBuffer for the compute shader
	struct BlurSettings
	{
		UINT blurRadius;
		bool useVerticalBlur;
		float padding[2];
		float weights[MAXWEIGHTS];
	} blurSettings;

	BlurState currentState;

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
	void ChangeBlur(BlurState state, float sigma = 0.0f);
	//void ChangeBlur(float percentage);	//1.0f = 100% blur aka maxblur and 0.0f is no blur
	void Render();
};