#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"
#include <string>
#include <fstream>

class LightPass
{
private:
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11RenderTargetView* renderTarget;
	ID3D11ShaderResourceView* shaderResource;
	ID3D11Texture2D* renderedImage;
	std::string vShaderByteCode;

	bool LoadShaders(ID3D11Device*& device);
	bool CreateRenderTarget(ID3D11Device*& device);
	bool CreateFullScreenQuad(ID3D11Device*& device);
	bool CreateInputLayout(ID3D11Device*& device);

public:
	LightPass();
	virtual ~LightPass();

	bool Initialize(ID3D11Device*& device, const UINT& windowWidth, const UINT& windowHeight);
	void Render(ID3D11DeviceContext* context);
};