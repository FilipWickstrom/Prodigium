#include "ShadowHandler.h"
#include <fstream>

const bool ShadowHandler::LoadVertexShader()
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

void ShadowHandler::ClearHistory()
{
	if (this->vertexShader)
	{
		this->vertexShader->Release();
		this->vertexShader = nullptr;
	}

	if (this->shadowMapArray)
	{
		this->shadowMapArray->Release();
		this->shadowMapArray = nullptr;
	}

	if (this->lightArrayView)
	{
		this->lightArrayView->Release();
		this->lightArrayView = nullptr;
	}

	if (this->arrayView)
	{
		this->arrayView->Release();
		this->arrayView = nullptr;
	}

	if (this->lightInfoBuffer)
	{
		this->lightInfoBuffer->Release();
		this->lightInfoBuffer = nullptr;
	}
}

bool ShadowHandler::SetupMapArray()
{
	// Allows for dynamic input of shadows maps
	this->ClearHistory();

	this->LoadVertexShader();
	this->viewPort.TopLeftX = 0.0f;
	this->viewPort.TopLeftY = 0.0f;
	this->viewPort.Width = static_cast<float>(SHADOWWIDTH);
	this->viewPort.Height = static_cast<float>(SHADOWHEIGHT);
	this->viewPort.MinDepth = 0.0f;
	this->viewPort.MaxDepth = 1.0f;

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

	if (SUCCEEDED(hr))
	{
		hr2 = Graphics::GetDevice()->CreateShaderResourceView(this->shadowMapArray, &srvDesc, &this->arrayView);
	}
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

	return true;
}

ShadowHandler::ShadowHandler()
{
	this->newMap = true;
	this->shadowMapArray = nullptr;
	this->arrayView = nullptr;
	this->lightArrayView = nullptr;
	this->lightInfoBuffer = nullptr;
	this->vertexShader = nullptr;
	this->viewPort = {};
	this->nrOf = 0;
}

void ShadowHandler::Add(const LightStruct& L)
{
	ShadowMap* map = new ShadowMap();
	map->SetUp(L);
	this->shadows.push_back(map);

	MatrixLight lightMatrix;
	lightMatrix.view = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreateLookAt({ L.position.x, L.position.y + 65.f, L.position.z },
		{ L.position.x - 0.001f, -1.0f , L.position.z + 0.001f }, { 0.0f, 1.0f, 0.0f }));

	lightMatrix.projection = DirectX::XMMatrixTranspose(DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(0.5f * DirectX::XM_PI, (float)(SHADOWWIDTH / SHADOWHEIGHT), 65.0f, 110.0f));

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
	if (this->vertexShader)
		this->vertexShader->Release();

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
	this->shadows[index]->RenderStatic();
}

const int ShadowHandler::NrOfShadows() const
{
	return (int)shadows.size();
}

void ShadowHandler::RenderLightPass()
{
	this->shadows[0]->RenderLightPass();
	Graphics::GetContext()->PSSetShaderResources( 5/*BUFFER_COUNT + 2*/, 1, &this->lightArrayView);	//6
	Graphics::GetContext()->PSSetShaderResources(4/*BUFFER_COUNT + 1*/, 1, &this->arrayView);		//5
}

void ShadowHandler::Clear()
{
	ID3D11ShaderResourceView* nullSRV = nullptr;
	Graphics::GetContext()->PSSetShaderResources(5/*BUFFER_COUNT + 2*/, 1, &nullSRV);	//6
	Graphics::GetContext()->PSSetShaderResources(4/*BUFFER_COUNT + 1*/, 1, &nullSRV);	//5
	Graphics::SetMainWindowViewport();
}

void ShadowHandler::Prepare()
{
	if (this->newMap)
	{
		this->SetupMapArray();
		this->newMap = false;
	}
	Graphics::GetContext()->VSSetShader(this->vertexShader, nullptr, 0);
	Graphics::GetContext()->RSSetViewports(1, &this->viewPort);
	Graphics::GetContext()->PSSetShader(NULL, NULL, NULL);
}
