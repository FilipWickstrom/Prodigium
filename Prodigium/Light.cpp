#include "Light.h"

Light::Light()
{
	this->type = LightType::NONE;
	this->lightBuffer = nullptr;
}

Light::~Light()
{
	if (this->lightBuffer)
		this->lightBuffer->Release();
}

bool Light::MakeLight(LightStruct L)
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(LightStruct);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &L;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightBuffer);
	return !FAILED(result);
}

ID3D11Buffer*& Light::GetBuffer()
{
	return this->lightBuffer;
}
