#include "ParticleSystem.h"
#include <fstream>

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

	Graphics::GetContext()->VSSetShaderResources(0, 1, &this->particleView);
	Graphics::GetContext()->IASetInputLayout(this->inputLayout);

	/*
		Render Phase
	*/
	Graphics::BindBackBuffer();
	Graphics::GetContext()->DrawInstanced(1, MAX_PARTICLES, 0, 0);


	/*
		Update Phase
	*/
	Graphics::GetContext()->Dispatch(MAX_PARTICLES / 8, 1, 1);

	this->Clear();
}

void ParticleSystem::ClearHistory()
{
}

void ParticleSystem::Clear()
{
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
	this->inputLayout = nullptr;
	this->particleAccess = nullptr;
	this->particleBuff = nullptr;
	this->particleView = nullptr;
	this->hasSetup = false;
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
	if (this->inputLayout)
		this->inputLayout->Release();
	if (this->particleAccess)
		this->particleAccess->Release();
	if (this->particleBuff)
		this->particleBuff->Release();
	if (this->particleView)
		this->particleView->Release();
}

bool ParticleSystem::SetUp()
{
	this->ClearHistory();

	HRESULT hr;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		ParticleVertex part;
		part.position = DirectX::SimpleMath::Vector4(randomize(1000.0f, -1000.0f), randomize(125.0f, 75.0f), randomize(1000.0f, -1000.0f), 1.0f);
		this->parts.push_back(part);
	}

	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.ByteWidth = sizeof(ParticleVertex) * (int)this->parts.size();
	desc.StructureByteStride = sizeof(ParticleVertex);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &(this->parts[0]);

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->particleBuff);
	if (FAILED(hr))
		return false;

	hr = Graphics::GetDevice()->CreateUnorderedAccessView(this->particleBuff, 0, &this->particleAccess);
	if (FAILED(hr))
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.Flags = 0;
	srvDesc.BufferEx.NumElements = (int)this->parts.size();

	hr = Graphics::GetDevice()->CreateShaderResourceView(this->particleBuff, 0, &this->particleView);
	assert(SUCCEEDED(hr));
	if (FAILED(hr))
		return false;

	this->LoadGeometryShader();
	this->LoadVertexShader();
	this->LoadPixelShader();
	this->LoadComputeShader();

	D3D11_INPUT_ELEMENT_DESC inputDesc[1] = 
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
	hr = Graphics::GetDevice()->CreateInputLayout(inputDesc, 1, this->vertexData.c_str(), this->vertexData.length(), &this->inputLayout);
	if (FAILED(hr))
		return false;

	return true;
}

void ParticleSystem::Render()
{
	while (!hasSetup)
	{
		if (this->SetUp())
		{
			hasSetup = true;
		}
		else
			hasSetup = false;
	}
	//this->InternalRender();
}
