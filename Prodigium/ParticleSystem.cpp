#include "ParticleSystem.h"

float randomize(float upper, float lower)
{
	float down = (float)(rand() % (int)lower);
	float up = (float)(rand() % (int)upper + lower);
	return up - down;
}


void ParticleSystem::InternalRender()
{
}

void ParticleSystem::ClearHistory()
{
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
}

bool ParticleSystem::SetUp()
{
	this->ClearHistory();

	HRESULT hr;
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		ParticleVertex part;
		part.position = DirectX::SimpleMath::Vector3(randomize(1000.0f, 1000.0f), randomize(125.0f, 75.0f), randomize(1000.0f, 1000.0f));
		part.size = DirectX::SimpleMath::Vector2(randomize(10.0f, 5.0f) * 0.1f, randomize(10.0f, 5.0f) * 0.1f);
		this->parts.push_back(part);
	}

	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(ParticleVertex) * (int)this->parts.size();
	desc.StructureByteStride = sizeof(ParticleVertex);

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &(this->parts[0]);
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->particleBuff);

	return !FAILED(hr);
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
	this->InternalRender();
}
