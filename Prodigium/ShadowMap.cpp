#include "ShadowMap.h"

bool ShadowMap::SetupShadowMap()
{
	HRESULT hr1, hr2, hr3;
	// Set up texture to be used by DSV and RSV
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = SHADOWWIDTH;
	texDesc.Height = SHADOWHEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr1 = Graphics::GetDevice()->CreateTexture2D(&texDesc, 0, &this->shadowMap);
	if (FAILED(hr1))
	{
		return false;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc;
	srDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = 1;

	hr2 = Graphics::GetDevice()->CreateShaderResourceView(this->shadowMap, &srDesc, &this->shadowResourceView);
	if (FAILED(hr2))
		return false;

	// Set up depth stencil
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	depthDesc.Flags = 0;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthDesc.Texture2D.MipSlice = 0;
	hr3 = Graphics::GetDevice()->CreateDepthStencilView(this->shadowMap, &depthDesc, &this->shadowRenderTarget);
	if (FAILED(hr3))
	{
		return false;
	}
    return true;
}

void ShadowMap::SetUp(const LightStruct& lightSt)
{
	if (!this->SetupShadowMap())
	{
		std::cout << "Setting up Shadow Map failed!\n";
	}
}

void ShadowMap::RenderStatic()
{
}

void ShadowMap::RenderDynamic()
{
}
