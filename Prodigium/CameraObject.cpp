#include "CameraObject.h"
using namespace DirectX::SimpleMath;
void CameraObject::UpdateViewMatrix()
{
	this->targetPos.Transform(defaultForward, rotationMatrix);
	this->targetPos += this->eyePos;
	//this->targetPos = XMVectorAdd(XMVector3TransformCoord(this->defaultForward, this->rotationMatrix), this->eyePos);
	this->upDir.Transform(defaultUp, rotationMatrix);
	//this->upDir = XMVector3TransformCoord(this->defaultUp, this->rotationMatrix);
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
	this->upDir = {};
	this->viewProjMatrix.viewMatrix = {};
	this->eyePos = { 0.f, 0.f, 0.f };
	this->pitch = 0.f;
	this->yaw = 0.f;
	this->roll = 0.f;
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
	this->targetPos = { 0.f,0.f,1.f };
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->fieldOfView = fov;
	this->aspectRatio = float(windowWidth) / float(windowHeight);
	this->upDir = { 0.f,1.f,0.f };
	this->viewProjMatrix.viewMatrix = XMMatrixTranspose(XMMatrixLookAtLH(eyePos, targetPos, upDir));
	//this->viewProjMatrix.viewMatrix.CreateLookAt(this->eyePos, this->targetPos, this->upDir);

	//this->viewProjMatrix.viewMatrix.Transpose();
	this->viewProjMatrix.projectionMatrix = XMMatrixTranspose(XMMatrixPerspectiveFovLH(this->fieldOfView, aspectRatio, this->nearPlane, this->farPlane));
	//this->viewProjMatrix.projectionMatrix.CreatePerspectiveFieldOfView(this->fieldOfView,((float)windowWidth / windowHeight), this->nearPlane, this->farPlane);
	//this->viewProjMatrix.projectionMatrix.Transpose();

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

//Matrix CameraObject::GetViewMatrix() const
//{
//	return this->viewProjMatrix.viewMatrix;
//}

//Matrix CameraObject::GetProjectionMatrix() const
//{
//	return this->viewProjMatrix.projectionMatrix;
//}

void CameraObject::Move(float x, float z)
{
	this->rotationMatrix.CreateFromYawPitchRoll(this->yaw, this->pitch, this->roll);
	this->eyePos += eyePos.Transform({ x,0.f,z }, this->rotationMatrix);

	//std::cout << eyePos.x << " " << eyePos.y << " " << eyePos.z << std::endl;
	//this->eyePos = XMVectorAdd(this->eyePos, XMVector3TransformCoord({ x, 1.f, z }, this->rotationMatrix));
	this->UpdateViewMatrix();
}

void CameraObject::Move(float x, float y, float z)
{
	this->rotationMatrix.CreateFromYawPitchRoll(this->yaw, this->pitch, this->roll);
	this->eyePos.Transform({ x,y,z }, this->rotationMatrix);
	//this->eyePos = XMVectorAdd(this->eyePos, XMVector3TransformCoord({ x, 1.f, z }, this->rotationMatrix));
	this->UpdateViewMatrix();
}

void CameraObject::Rotate(float pitchAmount, float yawAmount)
{
	this->pitch = fmod(this->pitch + pitchAmount, FULL_CIRCLE);
	this->yaw = fmod(this->yaw + yawAmount, FULL_CIRCLE);
	this->UpdateViewMatrix();
}

void CameraObject::SetPosition(float xPos, float yPos)
{
	this->eyePos = { xPos,yPos, 1.f };
	this->UpdateViewMatrix();
}

Vector3 CameraObject::getPos() const
{
	return this->eyePos;
}

void CameraObject::Bind()
{
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &matrixBuffer);
}

void CameraObject::Update()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	Graphics::GetContext()->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &viewProjMatrix, sizeof(viewProjMatrix));
	Graphics::GetContext()->Unmap(matrixBuffer, 0);
}

//ID3D11Buffer*& CameraObject::GetViewProjMatrix()
//{
//	return this->matrixBuffer;
//}
