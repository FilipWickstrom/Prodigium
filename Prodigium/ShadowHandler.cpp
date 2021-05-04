#include "ShadowHandler.h"

bool ShadowHandler::SetupMapArray()
{
	// to do: compile all textures into map.
	HRESULT hr;
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
	srvDesc.Format = texDesc.Format;

	return SUCCEEDED(hr);
}

ShadowHandler::ShadowHandler()
{
	this->newMap = true;
	this->shadowMapArray = nullptr;
	this->arrayView = nullptr;
	this->nrOf = 0;
}

void ShadowHandler::Add(const LightStruct& L)
{
	ShadowMap* map = new ShadowMap();
	map->SetUp(L);
	shadows.push_back(map);
	this->newMap = true;
	this->nrOf++;
}

ShadowHandler::~ShadowHandler()
{
	if (this->arrayView)
		this->arrayView->Release();
	if (this->shadowMapArray)
		this->shadowMapArray->Release();

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
