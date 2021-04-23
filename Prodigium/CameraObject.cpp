#include "CameraObject.h"
using namespace DirectX::SimpleMath;
void CameraObject::UpdateViewProjection()
{
	this->targetPos = XMVectorAdd(XMVector3TransformCoord(this->defaultForward, this->rotationMatrix), this->eyePos);
	this->upDir = XMVector3TransformCoord(this->defaultUp, this->rotationMatrix);
	this->viewProjMatrix.viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(this->eyePos, this->targetPos, this->upDir));
}

CameraObject::CameraObject()
{
	this->aspectRatio = 0.f;
	this->nearPlane = 0.f;
	this->farPlane = 0.f;
	this->fieldOfView = 0.f;
	this->targetPos = {};
	this->viewProjMatrix.projectionMatrix = {};
	this->rotationMatrix = {};
	this->rotation = {};
	this->upDir = {};
	this->viewProjMatrix.viewMatrix = {};
	this->eyePos = { 0.f, 0.f, 0.f };
	this->pitch = 0.f;
	this->yaw = 0.f;
	this->matrixBuffer = nullptr;
}

CameraObject::~CameraObject()
{
	if (this->matrixBuffer)
		this->matrixBuffer->Release();
}

bool CameraObject::Initialize(int windowWidth, int windowHeight, float nearPlane, float farPlane, float fov, Vector3 position)
{
	this->eyePos = position;
	this->targetPos = { 0.f,0.f,0.f };
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->fieldOfView = fov;
	this->aspectRatio = float(windowWidth) / float(windowHeight);
	this->upDir = { 0.f,1.f,0.f };
	//this->viewProjMatrix.viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(eyePos, targetPos, upDir));
	this->viewProjMatrix.viewMatrix.CreateLookAt(this->eyePos, this->targetPos, this->upDir);
	this->viewProjMatrix.viewMatrix.Transpose();
	this->viewProjMatrix.projectionMatrix.CreatePerspectiveFieldOfView(this->fieldOfView, this->aspectRatio, this->nearPlane, this->farPlane);
	this->viewProjMatrix.projectionMatrix.Transpose();
	//this->viewProjMatrix.projectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(this->fieldOfView, aspectRatio, this->nearPlane, this->farPlane));
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


	return true;
}

Matrix CameraObject::GetViewMatrix() const
{
	return this->viewProjMatrix.viewMatrix;
}

Matrix CameraObject::GetProjectionMatrix() const
{
	return this->viewProjMatrix.projectionMatrix;
}

void CameraObject::Move(float x, float y)
{
	this->eyePos = XMVectorAdd(this->eyePos, XMVector3TransformCoord({ x, y, 1.f }, this->rotationMatrix));
	this->UpdateViewProjection();
}

void CameraObject::Rotate(float pitchAmount, float yawAmount)
{
	this->pitch = fmod(this->pitch + pitchAmount, FULL_CIRCLE);
	this->yaw = fmod(this->yaw + yawAmount, FULL_CIRCLE);
	this->UpdateViewProjection();
}

void CameraObject::SetPosition(float xPos, float yPos)
{
	this->eyePos = { xPos,yPos, 1.f };
	this->UpdateViewProjection();
}

ID3D11Buffer*& CameraObject::GetViewProjMatrix()
{
	return this->matrixBuffer;
}
