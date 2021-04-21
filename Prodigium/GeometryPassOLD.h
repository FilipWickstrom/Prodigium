#pragma once
#include <d3d11.h>
#include <string>
#include "UsefulStructuresHeader.h"
#include "ResourceManager.h"

class GeometryPassOLD
{
private:
	TextureRenderTargets gBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthTexture;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11SamplerState* sampler;
	std::string vShaderByteCode;

	bool CreateGBuffers(ID3D11Device*& device, const UINT& windowWidth, const UINT& windowHeight);
	bool CreateDepthBuffer(ID3D11Device*& device, const UINT& windowWidth, const UINT& windowHeight);
	bool CreateSamplerState(ID3D11Device*& device);
	bool LoadShaders(ID3D11Device*& device);
	bool CreateInputLayout(ID3D11Device*& device);
	
	// TESTING ENVIRONMENT
	ID3D11Buffer* vBuffer;
	bool CreateQuad(ID3D11Device*& devices);

public:
	GeometryPassOLD();
	~GeometryPassOLD();

	bool Initialize(ID3D11Device* device, const UINT& windowWidth, const UINT& windowHeight);
	void RenderGPass(ID3D11DeviceContext* context);
};