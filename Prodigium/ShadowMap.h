#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"
#define SHADOWHEIGHT 512
#define SHADOWWIDTH 512
class ShadowMap
{
private:

	ID3D11Texture2D* shadowMap;
	ID3D11RenderTargetView* shadowRenderTarget;
	ID3D11ShaderResourceView* shadowResourceView;

	bool SetupShadowMap();

public:

	// Setup the shadow map.
	void SetUp(const LightStruct &lightSt);

	// Render all the static objects once.
	void RenderStatic();

	// Render the dynamic object moving around.
	void RenderDynamic();
};

/*
	To do:
Check distance between object and light. If within certain range include it into calculation.
Only render static object once since they do not move. Player is going to be on position 0 and monster on 1.
*/

