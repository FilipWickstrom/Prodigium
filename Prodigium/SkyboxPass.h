#pragma once
#include <d3d11.h>
#include <stb/stb_image.h>
#include <SimpleMath.h>

#include "RenderPass.h"
constexpr int SKYBOXSIDES = 6;

/*
Skybox made with cubemapping
Follows the camera and should not be possible
"walk" outside of the cube
*/

class SkyboxPass : public RenderPass
{
private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	int nrOfIndices;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11SamplerState* sampler;
	ID3D11Texture2D* combinedTextures;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11InputLayout* inputLayout;
	std::string vertexShaderByteCode;
	ID3D11RasterizerState* noCullingRasterizer;
	ID3D11DepthStencilState* lessEqualState;

	bool LoadShaders();
	bool CreateVertIndBuffers();
	bool CreateTexture();
	bool CreateShaderResourceView();
	bool CreateSampler();
	bool CreateInputLayout();
	bool CreateRasterizer();
	bool CreateDepthState();

public:
	SkyboxPass();
	~SkyboxPass();

	// Inherited via RenderPass
	virtual bool Initialize() override;

	virtual void Clear() override;

	virtual void Prepare() override;

};