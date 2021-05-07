#pragma once
#include <d3d11.h>
#include <fstream>
#include <vector>
#include "Graphics.h"

const double PI = 3.14159265359;
const UINT MAXWEIGHTSIZE = 24;
const UINT MAXRADIUS = MAXWEIGHTSIZE / 2 - 1;
const UINT MINRADIUS = 1;

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
		float weights[MAXWEIGHTSIZE];
	} blurSettings;

	std::vector<std::vector<float>>allGaussFilters;
	
	bool useBlurFilter;
	UINT maxBlurRadius;

	//Creating directx buffers and shaders
	bool CreateComputeShader();
	bool CreateUnorderedAccessView();
	bool CreateSettingsBuffer();
	
	void GenerateGaussFilters();
	void SetWeights(UINT radius);

	void UpdateBlurSettings();
	void SwapBlurDirection();

	//Update the blur depending on players sanity
	void UpdateBlurRadius(float sanity);

public: 
	BlurFilter();
	~BlurFilter();

	bool Initialize(int maxBlurRadius = 5);
	void Render(float blurPercentage);

};