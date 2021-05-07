#include "ShadowMap.h"
#include <fstream>


const bool ShadowMap::SetupLightBuffer(const LightStruct& lightSt)
{
	struct lightPos
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
	}Lpos;

	Lpos.view = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreateLookAt({ lightSt.position.x, lightSt.position.y + 65.f, lightSt.position.z },
		{ lightSt.position.x - 0.001f, -1.0f , lightSt.position.z + 0.001f }, { 0.0f, 1.0f, 0.0f }));

	Lpos.proj = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(0.5f * DirectX::XM_PI, (float)(SHADOWWIDTH / SHADOWHEIGHT), 65.0f, 110.0f));


	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(lightPos);
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &Lpos;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightBuffer);
	return !FAILED(result);
}

const bool ShadowMap::UpdateLightBuffer(const LightStruct& lightSt)
{
	struct lightPos
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
	}Lpos;

	Lpos.view = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreateLookAt({ lightSt.position.x, lightSt.position.y, lightSt.position.z },
		{ lightSt.position.x - 0.001f, -1.0f , lightSt.position.z + 0.001f }, { 0.0f, 1.0f, 0.0f }));

	Lpos.proj = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(0.5f * DirectX::XM_PI, (float)(SHADOWWIDTH / SHADOWHEIGHT), 85.0f, 150.0f));

	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr = Graphics::GetContext()->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &Lpos, sizeof(lightPos));

	Graphics::GetContext()->Unmap(this->lightBuffer, 0);

	return !FAILED(hr);
}

ShadowMap::ShadowMap()
{
	this->shadowDepth = nullptr;
	this->lightBuffer = nullptr;
}

ShadowMap::~ShadowMap()
{
	if (this->shadowDepth)
		this->shadowDepth->Release();

	if (this->lightBuffer)
		this->lightBuffer->Release();
}

void ShadowMap::SetUp(const LightStruct& lightSt)
{
	if (!this->SetupLightBuffer(lightSt))
	{
		std::cout << "Setting up Light Buffer for Shadow Map failed!\n";
	}

	this->lightSt = lightSt;
}

void ShadowMap::Update(const LightStruct& lightSt)
{
	this->UpdateLightBuffer(lightSt);
}

void ShadowMap::SetUpDepthView(const int index, ID3D11Texture2D*& arrayTexture)
{
	if (this->shadowDepth)
	{
		this->shadowDepth->Release();
		this->shadowDepth = nullptr;
	}

	// Set up depth stencil
	HRESULT hr;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthDesc;
	depthDesc.Flags = 0;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthDesc.Texture2DArray.ArraySize = 1;
	depthDesc.Texture2DArray.MipSlice = 0;
	depthDesc.Texture2DArray.FirstArraySlice = index;
	hr = Graphics::GetDevice()->CreateDepthStencilView(arrayTexture, &depthDesc, &this->shadowDepth);
	assert(SUCCEEDED(hr));
}

void ShadowMap::RenderStatic()
{
	/*
	* For now testing.
	*/
	ID3D11RenderTargetView* cleanTargets[BUFFER_COUNT] = { nullptr };
	ID3D11DepthStencilView* nullDepth = nullptr;
	ID3D11ShaderResourceView* nullSRV = nullptr;
	Graphics::GetContext()->PSSetShaderResources(4, 1, &nullSRV);
	Graphics::GetContext()->ClearDepthStencilView(this->shadowDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	Graphics::GetContext()->VSSetConstantBuffers(3, 1, &this->lightBuffer);
	Graphics::GetContext()->OMSetRenderTargets(BUFFER_COUNT, cleanTargets, this->shadowDepth);
}

void ShadowMap::Clean()
{
	ID3D11RenderTargetView* cleanTargets[BUFFER_COUNT] = { nullptr };
	ID3D11DepthStencilView* nullDepth = nullptr;
	ID3D11Buffer* nuller = nullptr;
	Graphics::GetContext()->OMSetRenderTargets(BUFFER_COUNT, cleanTargets, nullDepth);
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &nuller);
	Graphics::SetMainWindowViewport();
}

void ShadowMap::RenderLightPass()
{
	Graphics::GetContext()->PSSetConstantBuffers(2, 1, &this->lightBuffer);
}

const DirectX::SimpleMath::Vector4& ShadowMap::GetPos() const
{
	return this->lightSt.position;
}
