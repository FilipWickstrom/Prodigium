#include "LightPassOLD.h"

bool LightPassOLD::LoadShaders(ID3D11Device*& device)
{
	HRESULT hr;

	std::string shaderData;
	std::ifstream reader;

	reader.open("VertexShaderLightPass.cso", std::ios::binary | std::ios::ate);
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
	reader.open("PixelShaderLightPass.cso", std::ios::binary | std::ios::ate);
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

bool LightPassOLD::CreateRenderTarget(ID3D11Device*& device)
{
	return false;
}

bool LightPassOLD::CreateFullScreenQuad(ID3D11Device*& device)
{
	HRESULT hr;

	Vertex fullScreenQuad[] =
	{
		{ { -1.0f, -1.0f, 0.0f }, { 0.0f, 1.0f } }, // BOTTOM LEFT
		{ { -1.0, 1.0f, 0.0f }, { 0.0f, 0.0f } },   // TOP LEFT
		{ { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },   // TOP RIGHT
		{ { 1.0f, -1.0f, 0.0f }, { 1.0f, 1.0f } }    // BOTTOM RIGHT
	};

	DWORD indices[] =
	{
		0, 1, 2,
		0, 2, 3
	};

	D3D11_BUFFER_DESC quadDesc = {};

	quadDesc.ByteWidth = sizeof(fullScreenQuad);
	quadDesc.Usage = D3D11_USAGE_IMMUTABLE;
	quadDesc.CPUAccessFlags = 0;
	quadDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = {};

	data.pSysMem = fullScreenQuad;

	hr = device->CreateBuffer(&quadDesc, &data, &vBuffer);

	if (FAILED(hr))
	{
		return false;
	}

	quadDesc.ByteWidth = sizeof(indices);
	quadDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data.pSysMem = indices;

	hr = device->CreateBuffer(&quadDesc, &data, &iBuffer);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool LightPassOLD::CreateInputLayout(ID3D11Device*& device)
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC layout[2] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = device->CreateInputLayout(layout, 2, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

LightPassOLD::LightPassOLD()
{
	this->pShader = nullptr;
	this->vShader = nullptr;
	this->renderedImage = nullptr;
	this->renderTarget = nullptr;
	this->shaderResource = nullptr;
	this->vBuffer = nullptr;
	this->iBuffer = nullptr;
	this->inputLayout = nullptr;
}

LightPassOLD::~LightPassOLD()
{
	if (this->pShader)
		this->pShader->Release();
	if (this->vShader)
		this->vShader->Release();
	if (this->renderedImage)
		this->renderedImage->Release();
	if (this->renderTarget)
		this->renderTarget->Release();
	if (this->shaderResource)
		this->shaderResource->Release();
	if (this->vBuffer)
		this->vBuffer->Release();
	if (this->iBuffer)
		this->iBuffer->Release();
	if (this->inputLayout)
		this->inputLayout->Release();
}

bool LightPassOLD::Initialize(ID3D11Device*& device, const UINT& windowWidth, const UINT& windowHeight)
{
	if (!LoadShaders(device))
	{
		return false;
	}

	if (!CreateFullScreenQuad(device))
	{
		return false;
	}

	if (!CreateInputLayout(device))
	{
		return false;
	}

	return true;
}

void LightPassOLD::Render(ID3D11DeviceContext* context)
{
	context->VSSetShader(vShader, NULL, 0);
	context->PSSetShader(pShader, NULL, 0);
	context->IASetInputLayout(inputLayout);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	context->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, offset);

	context->DrawIndexed(6, 0, 0);
}