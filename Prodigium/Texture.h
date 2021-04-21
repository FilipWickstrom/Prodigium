#pragma once
#include <d3d11.h>
#include <string>
#include "Resource.h"
#include "Graphics.h"

class Texture : public Resource
{
private:
	ID3D11Texture2D* texture;
	UINT width;
	UINT height;
	std::string filename;

public:
	Texture();
	virtual ~Texture();

	bool Initialize(std::string filename, UINT width = 1280, UINT height = 1024, D3D11_SUBRESOURCE_DATA* data = nullptr);
	ID3D11Texture2D* getTexture2D()const;
};