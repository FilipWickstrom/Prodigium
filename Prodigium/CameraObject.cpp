#include "CameraObject.h"
using namespace DirectX::SimpleMath;
void CameraObject::UpdateViewMatrix()
{
	this->viewProjMatrix.viewMatrix = XMMatrixTranspose(XMMatrixLookToLH(this->eyePos, this->camForward, this->defaultUp));
}

CameraObject::CameraObject()
{
	this->aspectRatio = 0.f;
	this->nearPlane = 0.f;
	this->farPlane = 0.f;
	this->fieldOfView = 0.f;
	this->viewProjMatrix.projectionMatrix = {};
	this->upDir = {};
	this->viewProjMatrix.viewMatrix = {};
	this->eyePos = { 0.f, 0.f, 0.f };
	this->pitch = 0.f;
	this->yaw = 0.f;
	this->roll = 0.f;
	this->eyePosGPU = {};
	this->matrixBuffer = nullptr;
	this->camPosBuffer = nullptr;
}

CameraObject::~CameraObject()
{
	if (this->matrixBuffer)
		this->matrixBuffer->Release();
	if (this->camPosBuffer)
		this->camPosBuffer->Release();
}

bool CameraObject::Initialize(const int& windowWidth, const int& windowHeight, const float& nearPlane, const float& farPlane, const float& fov, const Vector3& eyePosition, const Vector3& lookTo)
{
	this->defaultForward = lookTo;
	this->defaultPosition = eyePosition;
	this->eyePos = eyePosition;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->fieldOfView = fov;
	this->aspectRatio = float(windowWidth) / float(windowHeight);
	this->upDir = { 0.f,1.f,0.f };
	this->camForward = lookTo;
	this->viewProjMatrix.viewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eyePos, camForward, upDir));
	this->viewProjMatrix.projectionMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(this->fieldOfView, aspectRatio, this->nearPlane, this->farPlane));


	D3D11_BUFFER_DESC buffDesc = {};
	buffDesc.ByteWidth = sizeof(viewProjectionMatrix);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &this->viewProjMatrix;

	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&buffDesc, &data, &matrixBuffer);

	if (FAILED(hr))
	{
		std::cout << "Failed to create camera constant buffer" << std::endl;
		return false;
	}

	buffDesc.ByteWidth = sizeof(this->eyePosGPU);
	buffDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	buffDesc.Usage = D3D11_USAGE_DYNAMIC;

	this->eyePosGPU = Vector4(this->eyePos.x, this->eyePos.y, this->eyePos.z, 0.0f);

	data.pSysMem = &eyePosGPU;

	hr = Graphics::GetDevice()->CreateBuffer(&buffDesc, &data, &camPosBuffer);

	if (FAILED(hr))
	{
		std::cout << "Failed to create camera constant buffer" << std::endl;
		return false;
	}

	return true;
}

void CameraObject::Rotate(const float& pitchAmount, const float& yawAmount, const float& rollAmount)
{
	this->pitch = fmod(this->pitch + pitchAmount, FULL_CIRCLE);
	this->yaw = fmod(this->yaw + yawAmount, FULL_CIRCLE);
	this->roll = fmod(this->roll + rollAmount, FULL_CIRCLE);

	if (this->pitch > 1.0f)
	{
		this->pitch = 1.0f;
	}
	if (this->pitch < -0.49f)
	{
		this->pitch = -0.49f;
	}

	this->SetRotation({ this->pitch, this->yaw, this->roll });
}

Vector3 CameraObject::GetPos() const
{
	return this->eyePos;
}

void CameraObject::Update()
{
	this->viewProjMatrix.viewMatrix = XMMatrixTranspose(XMMatrixLookToLH(this->eyePos, this->camForward, this->defaultUp));

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	Graphics::GetContext()->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &viewProjMatrix, sizeof(viewProjMatrix));
	Graphics::GetContext()->Unmap(matrixBuffer, 0);

	this->eyePosGPU = Vector4(this->eyePos.x, this->eyePos.y, this->eyePos.z, 0.0f);
	Graphics::GetContext()->Map(camPosBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &this->eyePosGPU, sizeof(this->eyePosGPU));
	Graphics::GetContext()->Unmap(camPosBuffer, 0);

	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &matrixBuffer);
	Graphics::GetContext()->PSSetConstantBuffers(1, 1, &camPosBuffer);

	Graphics::GetContext()->GSSetConstantBuffers(0, 1, &matrixBuffer);
	Graphics::GetContext()->GSSetConstantBuffers(1, 1, &camPosBuffer);
}

void CameraObject::ChangeOffset(const Vector3& offset)
{
}

DirectX::SimpleMath::Vector3 CameraObject::GetForward()
{
	return this->camForward;
}

void CameraObject::SetTransform(const Matrix& transform)
{
	Matrix transformed = Matrix::CreateFromYawPitchRoll(this->GetRotation().y, this->GetRotation().x, this->GetRotation().z) * transform;
	//Matrix transformed = Matrix::CreateFromYawPitchRoll(this->GetRotation().y, this->GetRotation().x, this->GetRotation().z) * transform;
	this->eyePos = Vector3::Transform(this->defaultPosition, transformed);
	this->camForward = Vector3::TransformNormal(this->defaultForward, transformed);
	this->camForward.Normalize();
}