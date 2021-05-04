#include "ShadowHandler.h"

bool ShadowHandler::SetupMapArray()
{
	// to do: compile all textures into map.
	HRESULT hr, hr2, hr3;
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = SHADOWWIDTH;
	texDesc.Height = SHADOWHEIGHT;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = (UINT)this->nrOf;
	texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	hr = Graphics::GetDevice()->CreateTexture2D(&texDesc, nullptr, &this->shadowMapArray);
	assert(SUCCEEDED(hr));

	for (int i = 0; i < this->nrOf; i++)
	{
		this->shadows[i]->SetUpDepthView(i, this->shadowMapArray);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.ArraySize = (UINT)this->nrOf;
	srvDesc.Texture2DArray.FirstArraySlice = 0;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.MostDetailedMip = 0;

	hr2 = Graphics::GetDevice()->CreateShaderResourceView(this->shadowMapArray, &srvDesc, &this->arrayView);
	assert(SUCCEEDED(hr2));

	// Description for the buffer containing all the light information.
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(MatrixLight) * (UINT)this->nrOf;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.StructureByteStride = sizeof(MatrixLight);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &(this->structs)[0];
	hr3 = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightInfoBuffer);

	if (!(FAILED(hr3)))
	{
		HRESULT shr;
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.Flags = 0;
		srvDesc.BufferEx.NumElements = (UINT)this->nrOf;

		shr = Graphics::GetDevice()->CreateShaderResourceView(this->lightInfoBuffer, &srvDesc, &this->lightArrayView);
		assert(SUCCEEDED(shr));

		return !FAILED(shr);
	}

	return SUCCEEDED(hr2);
}

ShadowHandler::ShadowHandler()
{
	this->newMap = true;
	this->shadowMapArray = nullptr;
	this->arrayView = nullptr;
	this->lightArrayView = nullptr;
	this->lightInfoBuffer = nullptr;
	this->nrOf = 0;
}

void ShadowHandler::Add(const LightStruct& L)
{
	ShadowMap* map = new ShadowMap();
	map->SetUp(L);
	this->shadows.push_back(map);

	MatrixLight lightMatrix;
	lightMatrix.view = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreateLookAt({ L.position.x, L.position.y + 30, L.position.z },
		{ L.position.x - 0.001f, -1.0f , L.position.z + 0.001f }, { 0.0f, 1.0f, 0.0f }));

	lightMatrix.projection = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(0.5f * DirectX::XM_PI, (float)(SHADOWWIDTH / SHADOWHEIGHT), .25f, 200.0f));

	structs.push_back(lightMatrix);

	this->newMap = true;
	this->nrOf++;
}

ShadowHandler::~ShadowHandler()
{
	if (this->arrayView)
		this->arrayView->Release();
	if (this->shadowMapArray)
		this->shadowMapArray->Release();
	if (this->lightArrayView)
		this->lightArrayView->Release();
	if (this->lightInfoBuffer)
		this->lightInfoBuffer->Release();

	for (int i = 0; i < (int)shadows.size(); i++)
	{
		if (this->shadows[i])
			delete this->shadows[i];
	}
}

ShadowMap& ShadowHandler::GetShadow(const int index)
{
	return *this->shadows[index];
}

void ShadowHandler::Render(const int index)
{
	if (this->newMap)
	{
		this->SetupMapArray();
		this->newMap = false;
	}
	this->shadows[index]->RenderStatic();
}

const int ShadowHandler::NrOfShadows() const
{
	return (int)shadows.size();
}

void ShadowHandler::RenderLightPass()
{
	this->shadows[0]->RenderLightPass();
	Graphics::GetContext()->PSSetShaderResources(5, 1, &this->lightArrayView);
	Graphics::GetContext()->PSSetShaderResources(4, 1, &this->arrayView);
}

void ShadowHandler::Clear()
{
	ID3D11RenderTargetView* cleanTargets[BUFFER_COUNT] = { nullptr };
	ID3D11DepthStencilView* nullDepth = nullptr;
	ID3D11Buffer* nuller = nullptr;
	Graphics::GetContext()->OMSetRenderTargets(BUFFER_COUNT, cleanTargets, nullDepth);
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &nuller);
	Graphics::SetMainWindowViewport();
}
