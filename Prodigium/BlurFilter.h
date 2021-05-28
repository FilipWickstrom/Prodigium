#pragma once
#include <d3d11.h>
#include <fstream>
#include <vector>
#include "Graphics.h"

const double PI = 3.14159265359;
const UINT MAXWEIGHT = 8;
const UINT MAXRADIUS = MAXWEIGHT - 1;	//Bad for performance
const UINT MINRADIUS = 1;
const UINT BLURLEVELSIZE = 5;

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
	* Supports blur up to radius 7 but it
	  cost some frames...
*/

enum class BlurLevel
{
	SUPERLOW,			//Rad: 1 sigma: 0.5f
	LOW,				//Rad: 2 sigma: 2.0f
	MEDIUM,				//Rad: 3 sigma: 5.0f
	HIGH,				//Rad: 3 sigma: 10.0f
	HELLAHIGH,			//Rad: 4 sigma: 10.0f
	NOBLUR				
};

class BlurFilter
{
private:
	ID3D11ComputeShader* computeShader;
	ID3D11UnorderedAccessView* unorderedAccessView;	//Backbuffer
	ID3D11Buffer* settingsBuffer;

	//CBuffer for the compute shader
	struct BlurSettings
	{
		UINT blurRadius;
		bool useVerticalBlur;
		float padding[2];
		float weights[MAXWEIGHT] = {};
	} blurSettings;

	std::pair<UINT, float> blurLevels[BLURLEVELSIZE];
	BlurLevel currentBlurLevel;

private:
	//Creating directx buffers and shaders
	bool CreateComputeShader();
	bool CreateUnorderedAccessView();
	bool CreateSettingsBuffer();
	
	//Help functions
	void SetupBlurLevels();
	void GenerateGaussFilter(BlurLevel level);
	void UpdateBlurSettings();
	void SwapBlurDirection();

public: 
	BlurFilter();
	~BlurFilter();

	bool Initialize();
	void SetBlurLevel(BlurLevel level);
	void Render(ID3D11UnorderedAccessView* uav = nullptr);
};