#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Texture::Texture()
	:Resource(ResourceType::TEXTURE)
{
	this->texture = nullptr;
	this->shaderView = nullptr;
}

Texture::~Texture()
{
	if (this->texture)
		this->texture->Release();

	if (this->shaderView)
		this->shaderView->Release();
}

bool Texture::LoadTexture(ID3D11Device*& device, std::string filePath)
{
	// Processing
	int height, width, bpp;
	unsigned char* ImageData = stbi_load(filePath.c_str(), &width, &height, &bpp, STBI_rgb_alpha);
	if (!ImageData)
	{
		printf("Couldn't open File!\n");
	}
	else
	{
		D3D11_TEXTURE2D_DESC desc;
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_IMMUTABLE;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;

		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = ImageData;
		data.SysMemPitch = width * 4;
		data.SysMemSlicePitch = 0;

		if (FAILED(device->CreateTexture2D(&desc, &data, &this->texture)))
		{
			printf("Creating Texture failed!\n");
		}
		else
			device->CreateShaderResourceView(this->texture, nullptr, &this->shaderView);

		return true;
	}
}

ID3D11Texture2D*& Texture::GetTexture()
{
	return this->texture;
}

ID3D11ShaderResourceView*& Texture::GetShaderView()
{
	return this->shaderView;
}

bool Texture::Load()
{
	return false;
}
