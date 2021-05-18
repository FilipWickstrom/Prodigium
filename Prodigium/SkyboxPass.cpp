#include "SkyboxPass.h"

bool SkyboxPass::LoadShaders()
{
	HRESULT hr;
	std::string shaderData;
	std::ifstream reader;

	reader.open("SkyboxVertexShader.cso", std::ios::binary | std::ios::ate);
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

	this->vertexShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();

	// PixelShaderDeferred
	reader.open("SkyboxPixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = Graphics::GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &this->pixelShader);
	if (FAILED(hr))
	{
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

bool SkyboxPass::CreateVertIndBuffers()
{
	//Load in a quads vertices and indices. Only need position.
	//UV and normals not interesting, therefore no mesh import
	float scale = 1.0f;
	DirectX::SimpleMath::Vector3 vertices[] =
	{
		//Frontside
		{-scale,  scale,  scale},	//Top left		0
		{ scale,  scale,  scale},	//Top right		1
		{-scale, -scale,  scale},	//Down left		2
		{ scale, -scale,  scale},	//Down right	3
		//Backside
		{-scale,  scale, -scale},	//Top left		4
		{ scale,  scale, -scale},	//Top right		5
		{-scale, -scale, -scale},	//Down left		6
		{ scale, -scale, -scale}	//Down right	7
	};

	D3D11_BUFFER_DESC desc = {};
	desc.ByteWidth = sizeof(vertices);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.CPUAccessFlags = 0;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = vertices;

	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->vertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	//Will be used to make faces 
	unsigned short indices[] =
	{
		//Front
		0, 1, 3,
		3, 2, 0,
		//Back
		5, 4, 6,
		6, 7, 5,
		//Left
		4, 0, 2, 
		2, 6, 4,
		//Right
		1, 5, 7,
		7, 3, 1,
		//Top
		4, 5, 1,
		1, 0, 4,
		//Bottom
		2, 3, 7,
		7, 6, 2
	};

	this->nrOfIndices = sizeof(indices) / sizeof(unsigned short);
	desc.ByteWidth = sizeof(indices);
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	data.pSysMem = indices;

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->indexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool SkyboxPass::CreateTexture()
{
	int width;
	int height;
	int comp;
	int channels = STBI_rgb_alpha;
	unsigned char* image[SKYBOXSIDES];

	std::string folder = "Skybox";
	std::string faces[SKYBOXSIDES] =
	{
		"Textures/" + folder + "/posx.png",
		"Textures/" + folder + "/negx.png",
		"Textures/" + folder + "/posy.png",
		"Textures/" + folder + "/negy.png",
		"Textures/" + folder + "/posz.png",
		"Textures/" + folder + "/negz.png",
	};
	
	//Load in the images
	for (int i = 0; i < SKYBOXSIDES; i++)
	{
		image[i] = stbi_load(faces[i].c_str(), &width, &height, &comp, channels);
	}

	D3D11_TEXTURE2D_DESC desc = {};
	desc.ArraySize = SKYBOXSIDES;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.Height = height;
	desc.Width = width;
	desc.MipLevels = 1;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	
	D3D11_SUBRESOURCE_DATA data[SKYBOXSIDES] = {};
	for (int i = 0; i < SKYBOXSIDES; i++)
	{
		data[i].pSysMem = &image[i][0];
		data[i].SysMemPitch = static_cast<UINT>(width * channels);
		data[i].SysMemSlicePitch = 0;
	}

	HRESULT hr = Graphics::GetDevice()->CreateTexture2D(&desc, data, &this->combinedTextures);
	
	//Freeing up the images
	for (int i = 0; i < SKYBOXSIDES; i++)
	{
		stbi_image_free(image[i]);
	}

	if (FAILED(hr))
	{
		std::cout << "Failed to create texture 2d for skybox..." << std::endl;
		return false;
	}
	return true;
}

bool SkyboxPass::CreateShaderResourceView()
{
	D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
	desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	desc.Texture2D.MostDetailedMip = 0;
	desc.Texture2D.MipLevels = 1;

	HRESULT hr = Graphics::GetDevice()->CreateShaderResourceView(this->combinedTextures, &desc, &this->shaderResourceView);
	if (FAILED(hr))
	{
		std::cout << "Failed to create shader resource view for skybox..." << std::endl;
		return false;
	}

	return true;
}

bool SkyboxPass::CreateSampler()
{
	D3D11_SAMPLER_DESC samplerDesc = {};

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(Graphics::GetDevice()->CreateSamplerState(&samplerDesc, &this->sampler)))
	{
		return false;
	}
	return true;
}

bool SkyboxPass::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC layout[1] =
	{
		{"SKYPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	HRESULT hr = Graphics::GetDevice()->CreateInputLayout(layout, 1, this->vertexShaderByteCode.c_str(),
		this->vertexShaderByteCode.length(), &this->inputLayout);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool SkyboxPass::CreateRasterizer()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	desc.FrontCounterClockwise = false;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.SlopeScaledDepthBias = 0.0f;
	desc.DepthClipEnable = true;
	desc.ScissorEnable = false;
	desc.MultisampleEnable = false;
	desc.AntialiasedLineEnable = false;
	desc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;

	HRESULT hr = Graphics::GetDevice()->CreateRasterizerState(&desc, &this->noCullingRasterizer);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool SkyboxPass::CreateDepthState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	HRESULT hr = Graphics::GetDevice()->CreateDepthStencilState(&depthStencilDesc, &this->lessEqualState);
	if (FAILED(hr))
		return false;
	else
		return true;
}

SkyboxPass::SkyboxPass()
{
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->nrOfIndices = 0;
	this->vertexShader = nullptr;
	this->pixelShader = nullptr;
	this->sampler = nullptr;
	this->inputLayout = nullptr;
	this->combinedTextures = nullptr;
	this->shaderResourceView = nullptr;
	this->vertexShaderByteCode = "";
	this->noCullingRasterizer = nullptr;
	this->lessEqualState = nullptr;
}

SkyboxPass::~SkyboxPass()
{
	if (this->vertexBuffer)
		this->vertexBuffer->Release();
	if (this->indexBuffer)
		this->indexBuffer->Release();
	if (this->vertexShader)
		this->vertexShader->Release();
	if (this->pixelShader)
		this->pixelShader->Release();
	if (this->sampler)
		this->sampler->Release();
	if (this->inputLayout)
		this->inputLayout->Release();
	if (this->combinedTextures)
		this->combinedTextures->Release();
	if (this->shaderResourceView)
		this->shaderResourceView->Release();
	if (this->noCullingRasterizer)
		this->noCullingRasterizer->Release();
	if (this->lessEqualState)
		this->lessEqualState->Release();
}

bool SkyboxPass::Initialize()
{
	if (!LoadShaders())
	{
		return false;
	}

	if (!CreateVertIndBuffers())
	{
		return false;
	}

	if (!CreateTexture())
	{
		return false;
	}

	if (!CreateShaderResourceView())
	{
		return false;
	}

	if (!CreateSampler())
	{
		return false;
	}

	if (!CreateInputLayout())
	{
		return false;
	}

	if (!CreateRasterizer())
	{
		return false;
	}

	if (!CreateDepthState())
	{
		return false;
	}

	return true;
}

void SkyboxPass::Clear()
{
	ID3D11Buffer* vertexBufferNull = nullptr;
	ID3D11Buffer* indexBufferNull = nullptr;
	ID3D11VertexShader* vertexShaderNull = nullptr;
	ID3D11PixelShader* pixelShaderNull = nullptr;
	ID3D11SamplerState* samplerStateNull = nullptr;
	ID3D11InputLayout* inputLayoutNull = nullptr;
	ID3D11ShaderResourceView* shaderResourceViewNull = nullptr ;
	UINT stride = 0;
	UINT offset = 0;

	Graphics::GetContext()->IASetVertexBuffers(0, 1, &vertexBufferNull, &stride, &offset);
	Graphics::GetContext()->IASetIndexBuffer(indexBufferNull, DXGI_FORMAT::DXGI_FORMAT_UNKNOWN, 0);
	Graphics::GetContext()->IASetInputLayout(inputLayoutNull);
	Graphics::GetContext()->VSSetShader(vertexShaderNull, nullptr, 0);
	Graphics::GetContext()->RSSetState(nullptr);
	Graphics::GetContext()->PSSetShader(pixelShaderNull, nullptr, 0);
	Graphics::GetContext()->PSSetSamplers(0, 1, &samplerStateNull);
	Graphics::GetContext()->PSSetShaderResources(0, 1, &shaderResourceViewNull);
	Graphics::GetContext()->OMSetDepthStencilState(nullptr, 1);
}

void SkyboxPass::Prepare()
{	
	Graphics::GetContext()->IASetInputLayout(this->inputLayout);
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(DirectX::SimpleMath::Vector3);
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	Graphics::GetContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R16_UINT, offset);
	Graphics::GetContext()->VSSetShader(this->vertexShader, nullptr, 0);
	Graphics::GetContext()->PSSetShader(this->pixelShader, nullptr, 0);
	Graphics::GetContext()->PSSetSamplers(0, 1, &sampler);
	Graphics::GetContext()->PSSetShaderResources(0, 1, &this->shaderResourceView);
	Graphics::GetContext()->RSSetState(this->noCullingRasterizer);
	Graphics::GetContext()->OMSetDepthStencilState(this->lessEqualState, 1);

	Graphics::GetContext()->DrawIndexed(this->nrOfIndices, 0, 0);
}