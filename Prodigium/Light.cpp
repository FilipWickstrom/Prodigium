#include "Light.h"

Light::Light(LightType type)
{
	this->type = type;
	this->lightBuffer = nullptr;
}

Light::~Light()
{
	if (this->lightBuffer)
		this->lightBuffer->Release();
}

bool Light::MakeDirectional(DirectionalLight dirL)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(DirectionalLight);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &dirL;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightBuffer);
	return !FAILED(result);
}

bool Light::MakeSpotLight(SpotLight spot)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(SpotLight);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &spot;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightBuffer);
	return !FAILED(result);
}

bool Light::MakePointLight(PointLight point)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(PointLight);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &point;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightBuffer);
	return !FAILED(result);
}

ID3D11Buffer*& Light::GetBuffer()
{
	return this->lightBuffer;
}
