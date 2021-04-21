#include "RenderPass.h"

RenderPass::RenderPass()
{
}

RenderPass::~RenderPass()
{
}

GeometryPass::GeometryPass()
{
	this->depthTexture = nullptr;
	this->depthStencilView = nullptr;
	this->inputLayout = nullptr;
	this->pShader = nullptr;
	this->sampler = nullptr;
	this->vShader = nullptr;

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		this->gBuffer.renderTargets[i] = nullptr;
		this->gBuffer.shaderResourceViews[i] = nullptr;
		this->gBuffer.textures[i] = new Texture();
	}

	//TESTING 
	this->vBuffer = nullptr;
}

GeometryPass::~GeometryPass()
{
	if (this->depthTexture)
		this->depthTexture->Release();
	if (this->depthStencilView)
		this->depthStencilView->Release();
	if (this->inputLayout)
		this->inputLayout->Release();
	if (this->pShader)
		this->pShader->Release();
	if (this->vShader)
		this->vShader->Release();
	if (this->sampler)
		this->sampler->Release();
	if (this->vBuffer)
		this->vBuffer->Release();
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		if (this->gBuffer.renderTargets[i])
			this->gBuffer.renderTargets[i]->Release();
		if (this->gBuffer.shaderResourceViews[i])
			this->gBuffer.shaderResourceViews[i]->Release();
	}
}

bool GeometryPass::CreateQuad()
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

	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&bufferDesc, &data, &vBuffer);

	return true;
}

bool GeometryPass::CreateGBuffer()
{
	HRESULT hr;

	D3D11_RENDER_TARGET_VIEW_DESC renderTargetDesc = {};

	renderTargetDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetDesc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc = {};

	shaderResourceDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	shaderResourceDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;


	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		gBuffer.textures[i]->GetTexture2DAddr() = ResourceManager::GetTexture(std::string(std::to_string(i)));

		hr = Graphics::GetDevice()->CreateRenderTargetView(gBuffer.textures[i]->GetTexture2D(), &renderTargetDesc, &gBuffer.renderTargets[i]);

		if (FAILED(hr))
		{
			return false;
		}

		hr = Graphics::GetDevice()->CreateShaderResourceView(gBuffer.textures[i]->GetTexture2D(), &shaderResourceDesc, &gBuffer.shaderResourceViews[i]);

		if (FAILED(hr))
		{
			return false;
		}
	}

	return true;
}

bool GeometryPass::CreateDepthBuffer()
{
	HRESULT hr;

	D3D11_TEXTURE2D_DESC depthDesc = {};

	depthDesc.Width = Graphics::GetWindowWidth();
	depthDesc.Height = Graphics::GetWindowHeight();
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

	hr = Graphics::GetDevice()->CreateTexture2D(&depthDesc, nullptr, &depthTexture);

	if (FAILED(hr))
	{
		return false;
	}

	hr = Graphics::GetDevice()->CreateDepthStencilView(depthTexture, &depthStencilDesc, &depthStencilView);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool GeometryPass::CreateInputLayout()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC geometryLayout[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = Graphics::GetDevice()->CreateInputLayout(geometryLayout, 3, this->vShaderByteCode.c_str(), this->vShaderByteCode.length(), &inputLayout);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool GeometryPass::LoadShaders()
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

	hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader);

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

	hr = Graphics::GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader);

	if (FAILED(hr))
	{
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

bool GeometryPass::Initialize()
{
	if (!LoadShaders())
	{
		return false;
	}

	if (!CreateGBuffer())
	{
		return false;
	}

	if (!CreateInputLayout())
	{
		return false;
	}

	if (!CreateDepthBuffer())
	{
		return false;
	}

	if (!CreateQuad())
	{
		return false;
	}

	return true;
}

void GeometryPass::Clear()
{
	ID3D11RenderTargetView* nullRenderTargets[BUFFER_COUNT] = { nullptr };

	Graphics::GetContext()->IASetVertexBuffers(0, 1, nullptr, nullptr, nullptr);
	Graphics::GetContext()->VSSetShader(nullptr, nullptr, 0);
	Graphics::GetContext()->PSSetShader(nullptr, nullptr, 0);
	Graphics::GetContext()->IASetInputLayout(nullptr);
	Graphics::GetContext()->OMSetRenderTargets(BUFFER_COUNT, nullRenderTargets, nullptr);
}

void GeometryPass::Prepare()
{
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	Graphics::GetContext()->VSSetShader(vShader, NULL, 0);
	Graphics::GetContext()->PSSetShader(pShader, NULL, 0);
	Graphics::GetContext()->IASetInputLayout(inputLayout);
	Graphics::GetContext()->OMSetRenderTargets(BUFFER_COUNT, gBuffer.renderTargets, depthStencilView);
	Graphics::GetContext()->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);

	// TEST
	Graphics::GetContext()->Draw(6, 0);
	// TEST END
}

bool LightPass::LoadShaders()
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

	hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader);

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

	hr = Graphics::GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader);
	if (FAILED(hr))
	{
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

bool LightPass::CreateFullScreenQuad()
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

	hr = Graphics::GetDevice()->CreateBuffer(&quadDesc, &data, &vBuffer);

	if (FAILED(hr))
	{
		return false;
	}

	quadDesc.ByteWidth = sizeof(indices);
	quadDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	data.pSysMem = indices;

	hr = Graphics::GetDevice()->CreateBuffer(&quadDesc, &data, &iBuffer);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool LightPass::CreateInputLayout()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC layout[2] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = Graphics::GetDevice()->CreateInputLayout(layout, 2, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

LightPass::LightPass()
{
	this->iBuffer = nullptr;
	this->inputLayout = nullptr;
	this->pShader = nullptr;
	this->renderedImage = nullptr;
	this->renderTarget = nullptr;
	this->shaderResource = nullptr;
	this->vBuffer = nullptr;
	this->vShader = nullptr;
	this->pShader = nullptr;
}

LightPass::~LightPass()
{
	if (this->iBuffer)
		this->iBuffer->Release();
	if (this->inputLayout)
		this->inputLayout->Release();
	if (this->renderedImage)
		this->renderedImage->Release();
	if (this->renderTarget)
		this->renderTarget->Release();
	if (this->shaderResource)
		this->shaderResource->Release();
	if (this->vBuffer)
		this->vBuffer->Release();
	if (this->vShader)
		this->vShader->Release();
	if (this->pShader)
		this->pShader->Release();
}

bool LightPass::Initialize()
{
	if (!LoadShaders())
	{
		return false;
	}

	if (!CreateFullScreenQuad())
	{
		return false;
	}

	if (!CreateInputLayout())
	{
		return false;
	}

	return true;
}

void LightPass::Clear()
{
}

void LightPass::Prepare()
{
	Graphics::GetContext()->VSSetShader(vShader, NULL, 0);
	Graphics::GetContext()->PSSetShader(pShader, NULL, 0);
	Graphics::GetContext()->IASetInputLayout(inputLayout);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	Graphics::GetContext()->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, offset);

	Graphics::GetContext()->DrawIndexed(6, 0, 0);
}