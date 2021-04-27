#pragma once
#include <d3d11.h>
#include <stb/stb_image.h>
#include <SimpleMath.h>

#include "RenderPass.h"
#define SKYBOXSIDES 6

class SkyboxPass : public RenderPass
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int nrOfIndices;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11SamplerState* sampler;		//Does not have to be anisotropic. MIN_MAG_MIP_LINEAR, WRAP
	ID3D11Texture2D* combinedTextures;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11InputLayout* inputLayout;
	std::string vertexShaderByteCode;

	ID3D11DepthStencilState* lessEqualDepthState;
	//ID3D11RasterizerState* noCullingRasterizer;

	bool LoadShaders();
	bool CreateVertIndBuffers();
	bool CreateTexture();
	bool CreateShaderResourceView();
	bool CreateSampler();
	bool CreateInputLayout();
	bool CreateDepthState();
	//bool CreateRasterizer();

public:
	SkyboxPass();
	~SkyboxPass();

	// Inherited via RenderPass
	virtual bool Initialize() override;

	virtual void Clear() override;

	virtual void Prepare() override;

};