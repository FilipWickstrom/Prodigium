#pragma once
#include <d3d11.h>
#include <string>
#include "Resource.h"
#include "Graphics.h"

class Texture : public Resource
{
private:
	UINT width;
	UINT height;
	std::string filename;

public:
	Texture();
	virtual ~Texture();

	ID3D11Texture2D* texture;
	bool Initialize(std::string filename, UINT width = Graphics::GetWindowWidth(), UINT height = Graphics::GetWindowHeight(), D3D11_SUBRESOURCE_DATA* data = nullptr);
	ID3D11Texture2D* GetTexture2D()const;
	ID3D11Texture2D*& GetTexture2DAddr();
	void SetTexture(ID3D11Texture2D* texture);
};