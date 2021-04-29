#pragma once
#include <d3d11.h>
#include <string>
#include "UsefulStructuresHeader.h"
#include <fstream>
#include "ResourceManager.h"
#include "Graphics.h"

class RenderPass
{
private:

public:
	RenderPass();
	virtual ~RenderPass();

	virtual bool Initialize() = 0;
	virtual void Clear() = 0;
	virtual void Prepare() = 0;
};

class LightPass : public RenderPass
{
private:
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11RenderTargetView* renderTarget;
	ID3D11ShaderResourceView* shaderResources[BUFFER_COUNT];
	ID3D11Texture2D* renderedImage;
	ID3D11SamplerState* sampler;
	std::string vShaderByteCode;

	bool LoadShaders();
	bool CreateShaderResources();
	bool CreateFullScreenQuad();
	bool CreateInputLayout();
	bool CreateSamplerState();

public:
	LightPass();
	virtual ~LightPass();

	// Inherited via RenderPass
	virtual bool Initialize() override;
	virtual void Clear() override;
	virtual void Prepare() override;
};

class GeometryPass :public RenderPass
{
private:
	TextureRenderTargets gBuffer;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11Texture2D* depthTexture;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11SamplerState* sampler;
	ID3D11InputLayout* inputLayout;
	std::string vShaderByteCode;

	bool CreateGBuffer();
	bool CreateDepthBuffer();
	bool CreateInputLayout();
	bool LoadShaders();
	bool CreateSamplerState();

public:
	GeometryPass();
	~GeometryPass();

	// Inherited via RenderPass
	virtual bool Initialize() override;

	virtual void Clear() override;

	virtual void Prepare() override;

	ID3D11DepthStencilView*& GetDepthStencilView();
};