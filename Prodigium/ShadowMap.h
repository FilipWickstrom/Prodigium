#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"
#define SHADOWHEIGHT 8192
#define SHADOWWIDTH 8192
class ShadowMap
{
private:

	ID3D11Texture2D* shadowMap;
	ID3D11DepthStencilView* shadowDepth;
	ID3D11ShaderResourceView* shadowResourceView;
	ID3D11Buffer* lightBuffer;
	D3D11_VIEWPORT viewPort;
	ID3D11VertexShader* vertexShader;
	// Internal setting up shadow map.
	bool SetupShadowMap();
	bool SetupLightBuffer(const LightStruct& lightSt);
	bool UpdateLightBuffer(const LightStruct& lightSt);
	bool LoadVertexShader();

public:

	ShadowMap();
	virtual ~ShadowMap();

	// Setup the shadow map.
	void SetUp(const LightStruct &lightSt);
	void Update(const LightStruct& lightSt);

	// Render all the static objects once.
	void RenderStatic();

	void Clean();

	// Render the dynamic object moving around.
	void RenderDynamic();

	void RenderLightPass();

	void test();
};

/*
	To do:
Check distance between object and light. If within certain range include it into calculation.
Only render static object once since they do not move. Player is going to be on position 0 and monster on 1.
*/

