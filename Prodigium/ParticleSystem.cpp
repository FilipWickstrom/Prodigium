#include "ParticleSystem.h"
#include <fstream>
#include "ResourceManager.h"

float randomize(float upper, float lower)
{
	float ret = (float)(rand() % (int)(upper - lower)) + lower;
	return ret;
}

void ParticleSystem::InternalRender()
{
	ID3D11Buffer* nullBuff = nullptr;
	UINT stride = 0;
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &nullBuff, &stride, &offset);
	Graphics::GetContext()->VSSetShader(this->vertexShader, nullptr, 0);
	Graphics::GetContext()->PSSetShader(this->pixelShader, nullptr, 0);
	Graphics::GetContext()->GSSetShader(this->geoShader, nullptr, 0);
	Graphics::GetContext()->CSSetShader(this->computeShader, nullptr, 0);
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	Graphics::GetContext()->VSSetShaderResources(0, 1, &this->particleView);

	ID3D11InputLayout* nullLayout = nullptr;
	Graphics::GetContext()->IASetInputLayout(nullLayout);

	/*
		Render Phase
	*/
	Graphics::GetContext()->OMSetDepthStencilState(nullptr, 0);
	Graphics::GetContext()->OMSetBlendState(this->alphaBlendState, 0, 0xffffffff);
	Graphics::GetContext()->CSSetConstantBuffers(0, 1, &this->speedBuffer);
	Graphics::GetContext()->PSSetShaderResources(0, 1, &this->albedoView);
	Graphics::GetContext()->PSSetShaderResources(1, 1, &this->opacityView);
	Graphics::GetContext()->PSSetSamplers(0, 1, &this->sampler);
	Graphics::GetContext()->DrawInstanced(1, MAX_PARTICLES, 0, 0);



	/*
		Update Phase
	*/
	if (!GUIHandler::IsPaused())
	{
		ID3D11ShaderResourceView* nullSRV = nullptr;
		Graphics::GetContext()->VSSetShaderResources(0, 1, &nullSRV);
		Graphics::GetContext()->CSSetUnorderedAccessViews(0, 1, &this->particleAccess, 0);
		Graphics::GetContext()->Dispatch(MAX_PARTICLES / 1024, 1, 1);
	}

	this->Clear();
}

void ParticleSystem::Clear()
{
	ID3D11VertexShader* nullVShader = nullptr;
	ID3D11UnorderedAccessView* nullAccess = nullptr;
	ID3D11ShaderResourceView* nullShader = nullptr;
	Graphics::GetContext()->VSSetShader(nullVShader, nullptr, 0);
	Graphics::GetContext()->PSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->GSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->CSSetShader(NULL, NULL, NULL);
	Graphics::GetContext()->PSSetShaderResources(0, 1, &nullShader);
	Graphics::GetContext()->PSSetShaderResources(1, 1, &nullShader);
	Graphics::GetContext()->OMSetBlendState(NULL, NULL, 0xffffffff);
	Graphics::GetContext()->CSSetUnorderedAccessViews(0, 1, &nullAccess, 0);

}

bool ParticleSystem::LoadVertexShader()
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("VertexShaderParticle.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	// Compiles shader data from .cso file
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &this->vertexShader)))
	{
		std::cerr << "Failed to create vertex shader!" << std::endl;
		return false;
	}

	this->vertexData = shaderData;

	shaderData.clear();
	reader.close();
	return true;
}

bool ParticleSystem::LoadGeometryShader()
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("GeometryShaderParticle.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	// Compiles shader data from .cso file
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(Graphics::GetDevice()->CreateGeometryShader(shaderData.c_str(), shaderData.length(), nullptr, &this->geoShader)))
	{
		std::cerr << "Failed to create geometry shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();
	return true;
}

bool ParticleSystem::LoadPixelShader()
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("PixelShaderParticle.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	// Compiles shader data from .cso file
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(Graphics::GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &this->pixelShader)))
	{
		std::cerr << "Failed to create pixel shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();
	return true;
}

bool ParticleSystem::LoadComputeShader()
{
	std::string shaderData;
	std::ifstream reader;
	reader.open("ComputeShaderParticle.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		std::cerr << "Could not open VS file!" << std::endl;
		return false;
	}

	// Compiles shader data from .cso file
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	if (FAILED(Graphics::GetDevice()->CreateComputeShader(shaderData.c_str(), shaderData.length(), nullptr, &this->computeShader)))
	{
		std::cerr << "Failed to create compute shader!" << std::endl;
		return false;
	}

	shaderData.clear();
	reader.close();
	return true;
}

ParticleSystem::ParticleSystem()
{
	this->geoShader = nullptr;
	this->vertexShader = nullptr;
	this->pixelShader = nullptr;
	this->computeShader = nullptr;
	this->particleAccess = nullptr;
	this->particleBuff = nullptr;
	this->particleView = nullptr;
	this->speedBuffer = nullptr;
	this->alphaBlendState = nullptr;
	this->albedoView = nullptr;
	this->opacityView = nullptr;
	this->rainAlbedoTexture = nullptr;
	this->rainOpacityTexture = nullptr;
	this->sampler = nullptr;
	this->hasSetup = false;
	this->isActive = true;
}

ParticleSystem::~ParticleSystem()
{
	if (this->geoShader)
		this->geoShader->Release();
	if (this->vertexShader)
		this->vertexShader->Release();
	if (this->pixelShader)
		this->pixelShader->Release();
	if (this->computeShader)
		this->computeShader->Release();
	if (this->particleAccess)
		this->particleAccess->Release();
	if (this->particleBuff)
		this->particleBuff->Release();
	if (this->particleView)
		this->particleView->Release();
	if (this->speedBuffer)
		this->speedBuffer->Release();
	if (this->alphaBlendState)
		this->alphaBlendState->Release();
	if (this->albedoView)
		this->albedoView->Release();
	if (this->opacityView)
		this->opacityView->Release();
	if (this->sampler)
		this->sampler->Release();
}

bool ParticleSystem::SetUp()
{
	std::vector<ParticleVertex> parts;
	HRESULT hr;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		ParticleVertex part;
		part.position = DirectX::SimpleMath::Vector3(randomize(1000.0f, -1000.0f), randomize(300.0f, 50.0f), randomize(1000.0f, -1000.0f));
		parts.push_back(part);
	}

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.ByteWidth = sizeof(ParticleVertex) * (int)parts.size();
	desc.StructureByteStride = sizeof(ParticleVertex);

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &(parts[0]);

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->particleBuff);
	if (FAILED(hr))
		return false;

	hr = Graphics::GetDevice()->CreateUnorderedAccessView(this->particleBuff, 0, &this->particleAccess);
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = (int)parts.size();

	hr = Graphics::GetDevice()->CreateShaderResourceView(this->particleBuff, 0, &this->particleView);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	this->LoadGeometryShader();
	this->LoadVertexShader();
	this->LoadPixelShader();
	this->LoadComputeShader();

	D3D11_BUFFER_DESC speedDesc = {};
	speedDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	speedDesc.Usage = D3D11_USAGE_DYNAMIC;
	speedDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	speedDesc.MiscFlags = 0;
	speedDesc.ByteWidth = sizeof(DirectX::SimpleMath::Vector4);

	hr = Graphics::GetDevice()->CreateBuffer(&speedDesc, 0, &this->speedBuffer);
	if (FAILED(hr))
		return false;

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.AlphaToCoverageEnable = true;
	blendDesc.IndependentBlendEnable = true;
	blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;

	this->rainAlbedoTexture = ResourceManager::GetTexture("Textures/raindrop_albedo.png");
	this->rainOpacityTexture = ResourceManager::GetTexture("Textures/raindrop_opacity.png");

	hr = Graphics::GetDevice()->CreateShaderResourceView(this->rainAlbedoTexture, NULL, &this->albedoView);
	hr = Graphics::GetDevice()->CreateShaderResourceView(this->rainOpacityTexture, NULL, &this->opacityView);

	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = 0;
	hr = Graphics::GetDevice()->CreateSamplerState(&samplerDesc, &this->sampler);


	hr = Graphics::GetDevice()->CreateBlendState(&blendDesc, &this->alphaBlendState);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool ParticleSystem::UpdateSpeedBuffer(DirectX::SimpleMath::Vector3 playerPos, DirectX::SimpleMath::Vector3 monsterPos)
{
	// Control the speed of particles.

	// Distance between player and monster
	float dist = (playerPos - monsterPos).Length();
	if (dist < 0)
		dist *= -1;

	// Changes the speed factor, highest being 1.0 aka normal speed.
	float factor = std::max(std::min(dist, 400.0f), 5.0f) * 0.25f;
	float speed = factor * 0.01f;

	DirectX::SimpleMath::Vector4 package = { speed, speed, speed, speed };
	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr = Graphics::GetContext()->Map(this->speedBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &package, sizeof(DirectX::SimpleMath::Vector4));
	Graphics::GetContext()->Unmap(this->speedBuffer, 0);

	return !FAILED(hr);
}

void ParticleSystem::SetActive(bool act)
{
	this->isActive = act;
}

void ParticleSystem::Render()
{
	if (isActive)
	{
		while (!hasSetup)
		{
			hasSetup = this->SetUp();
		}
		this->InternalRender();
	}
}


const bool ParticleSystem::IsActive() const
{
	return this->isActive;
}
