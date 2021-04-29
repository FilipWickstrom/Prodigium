#include "GameObject.h"
#include "Graphics.h"
using namespace DirectX::SimpleMath;
GameObject::GameObject()
{
	this->position = { 0.0f, 0.0f, 0.0f };
	this->scale = { 0.0f, 0.0f, 0.0f };
	this->rotation = { 0.0f, 0.0f, 0.0f };
	this->modelMatrixBuffer = nullptr;
	this->modelMatrix = { 1.0f, 0.0f, 0.0f, 0.0f,
											0.0f, 1.0f, 0.0f, 0.0f,
											0.0f, 0.0f, 1.0f, 0.0f,
											0.0f, 0.0f, 0.0f, 1.0f };
}

GameObject::~GameObject()
{
	if (this->modelMatrixBuffer)
		this->modelMatrixBuffer->Release();
}

bool GameObject::BuildMatrix(Vector3 pos, Vector3 scl, Vector3 rot)
{
	this->position = pos;
	this->scale = scl;
	this->rotation = rot;
	modelMatrix = Matrix(Matrix::CreateScale(this->scale) * Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z) * Matrix::CreateTranslation(position)).Transpose();

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(this->modelMatrix);
	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &this->modelMatrix;
	HRESULT result = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->modelMatrixBuffer);
	return !FAILED(result);
}

bool GameObject::UpdateMatrix(Vector3 pos, Vector3 scl, Vector3 rot)
{
	this->position = pos;
	this->scale = scl;
	this->rotation = rot;

	modelMatrix = Matrix(Matrix::CreateScale(this->scale) * Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z) * Matrix::CreateTranslation(position)).Transpose();

	D3D11_MAPPED_SUBRESOURCE submap;

	HRESULT hr = Graphics::GetContext()->Map(this->modelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &this->modelMatrix, sizeof(Matrix));

	Graphics::GetContext()->Unmap(this->modelMatrixBuffer, 0);

	return !FAILED(hr);
}

bool GameObject::UpdateMatrix()
{

	Matrix transformedCPU = Matrix::CreateScale(this->scale) * Matrix::CreateFromYawPitchRoll(this->rotation.y, this->rotation.x, this->rotation.z) * Matrix::CreateTranslation(this->position);


	this->modelMatrix = transformedCPU;
	Matrix transformedGPU = Matrix(transformedCPU).Transpose();


	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr;
	hr = Graphics::GetContext()->Map(this->modelMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &transformedGPU, sizeof(Matrix));

	Graphics::GetContext()->Unmap(this->modelMatrixBuffer, 0);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

Vector3 GameObject::GetPosition() const
{
	return this->position;
}

Vector3 GameObject::GetScale() const
{
	return this->scale;
}

Vector3 GameObject::GetRotation() const
{
	return this->rotation;
}

void GameObject::SetPosition(Vector3 newPos)
{
	this->position = newPos;
}

void GameObject::SetScale(Vector3 newScale)
{
	this->scale = newScale;
}

void GameObject::SetRotation(Vector3 newRotation)
{
	this->rotation = newRotation;
}

const Matrix& GameObject::GetModelMatrix()
{
	return this->modelMatrix;
}

Matrix GameObject::GetTransposedMatrix() const
{
	Matrix toBeReturned = this->modelMatrix;
	return toBeReturned.Transpose();
}

ID3D11Buffer*& GameObject::GetModelMatrixBuffer()
{
	return this->modelMatrixBuffer;
}

float GameObject::GetDistance(GameObject otherObject) const
{
	float dist = Vector3(otherObject.position - this->position).Length();

	if (dist < 0)
		dist *= -1;

	return dist;
}
