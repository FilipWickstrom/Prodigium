#include "Light.h"

Light::Light()
{
	this->type = LightType::NONE;
	this->lightSt = {};
	ZeroMemory(&this->lightSt, sizeof(this->lightSt));
}

Light::~Light()
{
}

bool Light::MakeLight(LightStruct L)
{
	this->lightSt = L;

	ID3D11UnorderedAccessView* test;

	ID3D11ShaderResourceView* srv;
	CD3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;


	// How to prepare the structured buffer
	/*
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(LightStruct);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &L;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightBuffer);
	return !FAILED(result);
	*/
	return true;
}