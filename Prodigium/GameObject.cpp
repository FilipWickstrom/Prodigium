#include "GameObject.h"
#include "Graphics.h"

GameObject::GameObject()
{
	this->position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->scale = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	this->modelMatrixBuffer = nullptr;
	this->modelMatrix = DirectX::XMFLOAT4X4(1.0f, 0.0f, 0.0f, 0.0f,
											0.0f, 1.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f);
}

GameObject::~GameObject()
{
	if (this->modelMatrixBuffer)
		this->modelMatrixBuffer->Release();
}

bool GameObject::BuildMatrix(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scl, DirectX::XMFLOAT3 rot)
{
	this->position = pos;
	this->scale = scl;
	this->rotation = rot;

	DirectX::XMStoreFloat4x4(&this->modelMatrix, DirectX::XMMatrixScaling(scl.x, scl.y, scl.z) * DirectX::XMMatrixRotationRollPitchYaw(rot.x, rot.y, rot.z) *
		DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z));
	
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(this->modelMatrix);
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &this->modelMatrix;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->modelMatrixBuffer);
	return !FAILED(result);
}

DirectX::XMFLOAT3 GameObject::GetPosition() const
{
	return this->position;
}

DirectX::XMFLOAT3 GameObject::GetScale() const
{
	return this->scale;
}

DirectX::XMFLOAT3 GameObject::GetRotation() const
{
	return this->rotation;
}

DirectX::XMFLOAT4X4 GameObject::GetModelMatrix() const
{
	return this->modelMatrix;
}

DirectX::XMMATRIX GameObject::GetTransposedMatrix() const
{
	DirectX::XMMATRIX toBeReturned = DirectX::XMLoadFloat4x4(&this->modelMatrix);
	return DirectX::XMMatrixTranspose(toBeReturned);
}

ID3D11Buffer*& GameObject::GetModelMatrixBuffer()
{
	return this->modelMatrixBuffer;
}

float GameObject::GetDistance(GameObject otherObject) const
{
	DirectX::XMVECTOR dist = DirectX::XMVector3Length(
		DirectX::XMVectorSubtract(
			DirectX::XMLoadFloat3(&otherObject.position), 
			DirectX::XMLoadFloat3(&this->position)));
	DirectX::XMFLOAT3 dist_fl;
	DirectX::XMStoreFloat3(&dist_fl, dist);

	float ret = dist_fl.x;

	if (ret < 0)
		ret *= -1;

	return ret;
}
