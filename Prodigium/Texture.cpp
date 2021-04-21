#include "Texture.h"
#include <iostream>

Texture::Texture()
	:Resource(ResourceType::TEXTURE)
{
	this->filename = filename;
	this->width = width;
	this->height = height;
	this->texture = nullptr;
}

Texture::~Texture()
{
	if (this->texture)
		this->texture->Release();
}

bool Texture::Initialize(std::string filename, UINT width, UINT height, D3D11_SUBRESOURCE_DATA* data)
{
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.MiscFlags = 0;
	// We only see one quad no need for more miplevels, use the first in the pyramid
	textureDesc.MipLevels = 1;
	// We only have 1 texture in the array of textures.
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;

	// Texture and srv for the shaders
	if (data)
	{
		textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		HRESULT hr = Graphics::GetDevice()->CreateTexture2D(&textureDesc, data, &texture);
		if (FAILED(hr))
		{
			std::cout << "WTF!?" << std::endl;
		}
	}
	else
	{
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		HRESULT hr = Graphics::GetDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);
		if (FAILED(hr))
		{
			std::cout << "Failed to create Texture2D for GBuffer!" << std::endl;
		}
	}

	return true;
}

ID3D11Texture2D* Texture::getTexture2D() const
{
	return this->texture;
}
