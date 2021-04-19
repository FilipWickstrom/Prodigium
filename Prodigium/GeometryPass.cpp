#include "GeometryPass.h"
#include <iostream>
#include <fstream>

bool GeometryPass::CreateGBuffers(ID3D11Device*& device, const UINT& windowWidth, const UINT& windowHeight)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC textureDesc = {};

	textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.Width = windowWidth;
	textureDesc.Height = windowHeight;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.SampleDesc.Count = 1;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc = {};

	renderTargetDesc.Format = textureDesc.Format;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};

	shaderResourceDesc.Format = textureDesc.Format;
	shaderResourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		hr = device->CreateTexture2D(&textureDesc, nullptr, &gBuffer.textures[i]);

		if (FAILED(hr))
		{
			return false;
		}

		hr = device->CreateRenderTargetView(gBuffer.textures[i], &renderTargetDesc, &gBuffer.renderTargets[i]);

		if (FAILED(hr))
		{
			return false;
		}

		hr = device->CreateShaderResourceView(gBuffer.textures[i], &shaderResourceDesc, &gBuffer.shaderResourceViews[i]);

		if (FAILED(hr))
		{
			return false;
		}
	}

	return true;
}

bool GeometryPass::CreateDepthBuffer(ID3D11Device*& device, const UINT& windowWidth, const UINT& windowHeight)
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC depthDesc = {};

	depthDesc.Width = windowWidth;
	depthDesc.Height = windowHeight;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.CPUAccessFlags = 0;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};

	depthStencilDesc.Format = depthDesc.Format;
	depthStencilDesc.Texture2D.MipSlice = 0;
	depthStencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	hr = device->CreateTexture2D(&depthDesc, nullptr, &depthTexture);

	if (FAILED(hr))
	{
		return false;
	}

	hr = device->CreateDepthStencilView(depthTexture, &depthStencilDesc, &depthStencilView);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool GeometryPass::CreateSamplerState(ID3D11Device*& device)
{
	HRESULT hr;

	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;

	hr = device->CreateSamplerState(&samplerDesc, &sampler);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool GeometryPass::LoadShaders(ID3D11Device*& device)
{
	HRESULT hr;

	std::string shaderData;
	std::ifstream reader;

	reader.open("VertexShaderGPass.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = device->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader);

	if (FAILED(hr))
	{
		return false;
	}

	this->vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();

	// PixelShaderDeferred
	reader.open("PixelShaderGPass.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = device->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader);
	if (FAILED(hr))
	{
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

bool GeometryPass::CreateInputLayout(ID3D11Device*& device)
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC geometryLayout[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = device->CreateInputLayout(geometryLayout, 3, this->vShaderByteCode.c_str(), this->vShaderByteCode.length(), &inputLayout);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool GeometryPass::CreateQuad(ID3D11Device*& device)
{
	Vertex quad[] =
	{
		{ { -0.5f, -0.5f, 0.0 }, { 0.0f, 1.0f }, { 0.f, 0.f, -1.0f} },
		{ {-0.5f, 0.5f, 0.0f }, { 0.0f, 0.0f }, { 0.0f, 0.0f, -1.0f} },
		{ { 0.5f, 0.5f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 0.0f, -1.0f } },

		{ { -0.5f, -0.5f, 0.0f}, { 0.0, 1.0f }, { 0.f, 0.f, -1.0f} },
		{ { 0.5f, 0.5f, 0.0f}, { 1.0, 0.0f }, { 0.f, 0.f, -1.0f} },
		{ { 0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, { 0.0f, 0.0f, -1.0f } },
	};

	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth = sizeof quad;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = quad;
	// Default to 0, only used by textures
	data.SysMemPitch = 0;
	// Only used in 3D for depth level to the next
	data.SysMemSlicePitch = 0;

	HRESULT hr = device->CreateBuffer(&bufferDesc, &data, &vBuffer);

	return true;
}

GeometryPass::GeometryPass()
{
	this->depthTexture = nullptr;
	this->depthStencilView = nullptr;
	this->inputLayout = nullptr;
	this->vShader = nullptr;
	this->pShader = nullptr;
	this->sampler = nullptr;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		gBuffer.renderTargets[i] = nullptr;
		gBuffer.shaderResourceViews[i] = nullptr;
		gBuffer.textures[i] = nullptr;
	}

	this->vBuffer = nullptr;
}

GeometryPass::~GeometryPass()
{
	if (this->depthStencilView)
		this->depthStencilView->Release();
	if (this->depthTexture)
		this->depthTexture->Release();
	if (this->inputLayout)
		this->inputLayout->Release();
	if (this->vShader)
		this->vShader->Release();
	if (this->pShader)
		this->pShader->Release();
	if (this->vBuffer)
		this->vBuffer->Release();
	if (this->sampler)
		this->sampler->Release();

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		if (this->gBuffer.renderTargets[i])
			this->gBuffer.renderTargets[i]->Release();
		if (this->gBuffer.shaderResourceViews[i])
			this->gBuffer.shaderResourceViews[i]->Release();
		if (this->gBuffer.textures[i])
			this->gBuffer.textures[i]->Release();
	}

	//Added***
	if (this->vBuffer)
		this->vBuffer->Release();
}

bool GeometryPass::Initialize(ID3D11Device* device, const UINT& windowWidth, const UINT& windowHeight)
{
	if (!LoadShaders(device))
	{
		return false;
	}

	if (!CreateInputLayout(device))
	{
		return false;
	}

	if (!CreateGBuffers(device, windowWidth, windowHeight))
	{
		return false;
	}

	if (!CreateDepthBuffer(device, windowWidth, windowHeight))
	{
		return false;
	}

	if (!CreateSamplerState(device))
	{
		return false;
	}
	
	if (!CreateQuad(device))
	{
		return false;
	}

	return true;
}

void GeometryPass::RenderGPass(ID3D11DeviceContext* context)
{
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	context->VSSetShader(vShader, NULL, 0);
	context->PSSetShader(pShader, NULL, 0);
	context->IASetInputLayout(inputLayout);
	context->OMSetRenderTargets(BUFFER_COUNT, gBuffer.renderTargets, depthStencilView);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	context->Draw(6, 0);
}