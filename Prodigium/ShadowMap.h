#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"
#define SHADOWHEIGHT 2048
#define SHADOWWIDTH 2048
class ShadowMap
{
private:

	ID3D11Texture2D* shadowMap;
	ID3D11DepthStencilView* shadowDepth;
	ID3D11ShaderResourceView* shadowResourceView;
	ID3D11Buffer* lightBuffer;
	D3D11_VIEWPORT viewPort;
	ID3D11VertexShader* vertexShader;
	LightStruct lightSt;
	// Internal setting up shadow map.
	const bool SetupShadowMap();
	const bool SetupLightBuffer(const LightStruct& lightSt);
	const bool UpdateLightBuffer(const LightStruct& lightSt);
	const bool LoadVertexShader();

public:

	ShadowMap();
	virtual ~ShadowMap();

	// Setup the shadow map.
	void SetUp(const LightStruct &lightSt);
	void Update(const LightStruct& lightSt);
	void SetUpDepthView(const int index, ID3D11Texture2D*& arrayTexture);

	// Render all the static objects once.
	void RenderStatic();

	void Clean();

	void RenderLightPass();

	const DirectX::SimpleMath::Vector4& GetPos() const;
};

