#include "ShadowMap.h"
#include <fstream>

const bool ShadowMap::SetupShadowMap()
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
	hr3 = Graphics::GetDevice()->CreateDepthStencilView(this->shadowMap, &depthDesc, &this->shadowDepth);
	if (FAILED(hr3))
	{
		return false;
	}

	this->viewPort.TopLeftX = 0.0f;
	this->viewPort.TopLeftY = 0.0f;
	this->viewPort.Width = static_cast<float>(SHADOWWIDTH);
	this->viewPort.Height = static_cast<float>(SHADOWHEIGHT);
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;

    return true;
}

const bool ShadowMap::SetupLightBuffer(const LightStruct& lightSt)
{
	struct lightPos
	{
		DirectX::SimpleMath::Matrix view;
		DirectX::SimpleMath::Matrix proj;
	}Lpos;

	Lpos.view = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreateLookAt({ lightSt.position.x, lightSt.position.y, lightSt.position.z },
		{ lightSt.position.x + lightSt.direction.x, lightSt.position.y + lightSt.direction.y, lightSt.position.z + lightSt.direction.z }, { 0.0f, 1.0f, 0.0f }));

	Lpos.proj = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(0.6f * DirectX::XM_PI, (float)(SHADOWWIDTH / SHADOWHEIGHT), 1.0f, 300.0f));

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
		{ lightSt.direction.x, lightSt.direction.y, lightSt.direction.z }, { 0.0f, 1.0f, 0.0f }));

	Lpos.proj = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(0.6f * DirectX::XM_PI, (float)(SHADOWWIDTH / SHADOWHEIGHT), 1.0f, 300.0f));

	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr = Graphics::GetContext()->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &Lpos, sizeof(lightPos));

	Graphics::GetContext()->Unmap(this->lightBuffer, 0);

	return !FAILED(hr);
}

const bool ShadowMap::LoadVertexShader()
{
	HRESULT hr;

	std::string shaderData;
	std::ifstream reader;

	reader.open("ShadowVertex.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &this->vertexShader);

	if (FAILED(hr))
	{
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

ShadowMap::ShadowMap()
{
	this->shadowMap = nullptr;
	this->shadowDepth = nullptr;
	this->shadowResourceView = nullptr;
	this->lightBuffer = nullptr;
	this->vertexShader = nullptr;
	this->viewPort = {};
}

ShadowMap::~ShadowMap()
{
	if (this->shadowDepth)
		this->shadowDepth->Release();

	if (this->lightBuffer)
		this->lightBuffer->Release();

	if (this->vertexShader)
		this->vertexShader->Release();
}

void ShadowMap::SetUp(const LightStruct& lightSt)
{
	if (!this->LoadVertexShader())
	{

	}
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

	Graphics::GetContext()->ClearDepthStencilView(this->shadowDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	Graphics::GetContext()->PSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &this->lightBuffer);
	Graphics::GetContext()->RSSetViewports(1, &this->viewPort);
	//Graphics::GetContext()->VSSetShader(this->vertexShader, nullptr, 0);
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

void ShadowMap::RenderDynamic()
{
}

void ShadowMap::RenderLightPass()
{
	Graphics::GetContext()->PSSetConstantBuffers(2, 1, &this->lightBuffer);
	//Graphics::GetContext()->PSSetShaderResources(4, 1, &this->shadowResourceView);
}

const DirectX::SimpleMath::Vector4& ShadowMap::GetPos() const
{
	return this->lightSt.position;
}
