#pragma once
#include <d3d11.h>
#include <fstream>
#include <vector>
#include "Graphics.h"

/*
Guassian blur filter that makes everything
blurry on the screen, but still so that you 
can see objects better than flat blur.
*/

class BlurFilter
{
private:
	ID3D11ComputeShader* computeShader;
	ID3D11UnorderedAccessView* unorderedAccessView;
	ID3D11Buffer* settingsBuffer;

	struct BlurSettings
	{
		UINT blurRadius;
		bool useVerticalBlur;
		float padding[2];
	} blurSettings;

	bool useBlurFilter;
	UINT maxBlurRadius;

	//std::vector<std::vector<float>> 
	//ID3D11ShaderResourceView* 

	//Creating directx buffers and shaders
	bool CreateComputeShader();
	bool CreateUnorderedAccessView();
	bool CreateSettingsBuffer();
	
	void UpdateBlurSettings();
	void SwapBlurDirection();

	//Update the blur depending on players sanity
	void UpdateBlurRadius(float sanity);

public: 
	BlurFilter();
	~BlurFilter();

	bool Initialize(UINT maxBlurRadius = 5);
	void Render(float blurPercentage);

};