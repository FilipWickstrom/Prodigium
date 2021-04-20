#pragma once
#include "Resource.h"
#include <d3d11.h>
#include <string>

class Texture : public Resource
{
private:

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* shaderView;

public:

	Texture();
	~Texture();

	bool LoadTexture(ID3D11Device*& device, std::string filePath);

	ID3D11Texture2D*& GetTexture();
	ID3D11ShaderResourceView*& GetShaderView();

	// Inherited via Resource
	virtual bool Load() override;

};