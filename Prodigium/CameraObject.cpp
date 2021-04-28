#include "CameraObject.h"
using namespace DirectX::SimpleMath;
void CameraObject::UpdateViewMatrix()
{
	//this->targetPos = this->targetPos.Transform(defaultForward, rotationMatrix);
	//this->targetPos += this->eyePos;
	this->upDir = this->upDir.Transform(defaultUp, rotationMatrix);
	this->viewProjMatrix.viewMatrix = XMMatrixTranspose(XMMatrixLookToLH(this->eyePos, this->camForward, this->defaultUp));
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

bool CameraObject::Initialize(int windowWidth, int windowHeight, float nearPlane, float farPlane, float fov, const Vector3& position, const Vector3& lookTo)
{
	this->defaultForward = lookTo;
	this->defaultPosition = position;
	//this->eyePos = position;
	this->targetPos = { 0.f,0.f,1.f };
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	this->fieldOfView = fov;
	this->aspectRatio = float(windowWidth) / float(windowHeight);
	this->upDir = { 0.f,1.f,0.f };
	//this->camForward = lookTo;
	this->viewProjMatrix.viewMatrix = DirectX::XMMatrixTranspose(DirectX::XMMatrixLookAtLH(eyePos, targetPos, upDir));
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


	return true;
}

void CameraObject::Move(float x, float z)
{
	this->rotationMatrix = this->rotationMatrix.CreateFromYawPitchRoll(this->yaw, this->pitch, this->roll);
	this->eyePos += eyePos.Transform({ x,0.f,z }, this->rotationMatrix);

	this->UpdateViewMatrix();
}

void CameraObject::Move(float x, float y, float z, XMFLOAT3 lookAt)
{
	this->rotationMatrix.CreateFromYawPitchRoll(this->yaw, this->pitch, this->roll);
	this->eyePos += eyePos.Transform({ x, y, z }, rotationMatrix);
	this->targetPos = lookAt;
	this->camForward = this->eyePos - this->targetPos;
	this->camForward.Normalize();

	this->UpdateViewMatrix();
}

void CameraObject::Rotate(float pitchAmount, float yawAmount)
{
	this->pitch = fmod(this->pitch + pitchAmount, FULL_CIRCLE);
	this->yaw = fmod(this->yaw + yawAmount, FULL_CIRCLE);
	this->rotationMatrix = this->rotationMatrix.CreateFromYawPitchRoll(this->yaw, this->pitch, this->roll);
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

void CameraObject::SetPosition(Vector3 newPos, Vector3 lookAt)
{
	this->eyePos = newPos;
	this->targetPos = lookAt;
	this->camForward = (this->eyePos - this->targetPos);
	this->camForward.Normalize();
	this->UpdateViewMatrix();
}

void CameraObject::BindCameraToPipeline()
{
	Graphics::GetContext()->VSSetConstantBuffers(0, 1, &matrixBuffer);
}

void CameraObject::Update()
{
	this->UpdateViewMatrix();

	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

	Graphics::GetContext()->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &viewProjMatrix, sizeof(viewProjMatrix));
	Graphics::GetContext()->Unmap(matrixBuffer, 0);

	this->BindCameraToPipeline();
}

void CameraObject::SetTransform(const Matrix& transform)
{
	this->eyePos = Vector3::Transform(this->defaultPosition, transform);
	Vector4 temp;
	temp = std::move(Vector4::Transform(Vector4(this->defaultForward.x, this->defaultForward.y, this->defaultForward.z, 0.0f), transform));
	this->camForward = std::move(Vector3(temp.x, temp.y, temp.z));
	this->camForward.Normalize();
}