#include "GameObject.h"
#include "Graphics.h"
#include <thread>
using namespace DirectX::SimpleMath;

GameObject::GameObject()
{
	this->position = { 0.0f, 0.0f, 0.0f };
	this->scale = { 0.0f, 0.0f, 0.0f };
	this->rotation = { 0.0f, 0.0f, 0.0f };
	this->up = { 0.0f, 1.0f, 0.0f };
	this->right = { 0.0f, 0.0f, 0.0f };
	this->modelMatrixBuffer = nullptr;
	this->modelMatrix = Matrix::Identity;
}

GameObject::~GameObject()
{
	if (this->modelMatrixBuffer)
		this->modelMatrixBuffer->Release();
}

bool GameObject::CreateModelMatrixBuffer()
{
	this->modelMatrix = this->modelMatrix.Transpose();

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(this->modelMatrix);
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &this->modelMatrix;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->modelMatrixBuffer);

	if (FAILED(result))
	{
		return false;
	}

	return true;
}

void GameObject::BuildMatrix(const Vector3& pos, const Vector3& scl, const Vector3& rot)
{
	this->position = pos;
	this->scale = scl;
	this->rotation = rot;
	this->modelMatrix = Matrix::CreateScale(this->scale) * 
						Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z) * 
						Matrix::CreateTranslation(position);
}

bool GameObject::UpdateMatrix(const Vector3& pos, const Vector3& scl, const Vector3& rot)
{
	this->position = pos;
	this->scale = scl;
	this->rotation = rot;

	this->modelMatrix = Matrix(Matrix::CreateScale(this->scale) * 
							   Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z) * 
							   Matrix::CreateTranslation(position)).Transpose();

	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr = Graphics::GetContext()->Map(this->modelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &this->modelMatrix, sizeof(Matrix));
	Graphics::GetContext()->Unmap(this->modelMatrixBuffer, 0);

	return !FAILED(hr);
}

bool GameObject::UpdateMatrix()
{
	Matrix transformedCPU = Matrix::CreateScale(this->scale) * 
							Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z) * 
							Matrix::CreateTranslation(this->position);

	this->modelMatrix = transformedCPU;

	//Update the buffer if it exists. Otherwise just locally
	if (this->modelMatrixBuffer)
	{
		Matrix transformedGPU = Matrix(transformedCPU).Transpose();

		D3D11_MAPPED_SUBRESOURCE submap;
		HRESULT hr = Graphics::GetContext()->Map(this->modelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
		memcpy(submap.pData, &transformedGPU, sizeof(DirectX::SimpleMath::Matrix));
		Graphics::GetContext()->Unmap(this->modelMatrixBuffer, 0);

		return !FAILED(hr);
	}

	return true;
}

void GameObject::UpdateMatrixCPU()
{
	this->modelMatrix = 
		Matrix::CreateScale(this->scale) * 
		Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z) * 
		Matrix::CreateTranslation(this->position);
}

const Vector3& GameObject::GetPosition() const
{
	return this->position;
}
const Vector3& GameObject::GetScale() const
{
	return this->scale;
}
const Vector3& GameObject::GetRotation() const
{
	return this->rotation;
}

void GameObject::SetPosition(const Vector3& newPos)
{
	this->position = newPos;
}

void GameObject::SetScale(const Vector3& newScale)
{
	this->scale = newScale;
}

void GameObject::SetRotation(const Vector3& newRotation)
{
	this->rotation = newRotation;
}

const Matrix& GameObject::GetModelMatrix() const
{
	return this->modelMatrix;
}

const Matrix GameObject::GetTransposedMatrix() const
{
	Matrix toBeReturned = this->modelMatrix;
	return toBeReturned.Transpose();
}

ID3D11Buffer*& GameObject::GetModelMatrixBuffer() 
{
	return this->modelMatrixBuffer;
}

float GameObject::GetDistance(const GameObject& otherObject) const
{
	float dist = Vector3(otherObject.position - this->position).Length();

	if (dist < 0)
	{
		dist *= -1;
	}

	return dist;
}

float GameObject::GetDistance(const Vector4& otherObject) const
{
	float dist = Vector3(otherObject - this->position).Length();
	if (dist < 0)
		dist *= -1;

	return dist;
}
