#include "Player.h"

using namespace DirectX::SimpleMath;

Player::Player()
{
	Vector3 position(0.0f, 0.0f, 0.f);
	Vector3 cameraOffset(0.0f, 0.0f, -0.2f);
	Vector3 cameraForward = position - cameraOffset;
	cameraForward.Normalize();
	this->speed = 5.f;
	this->playerModel = new MeshObject();
	this->playerModel->Initialize("LowPoly_Character.obj", "", "", position);
	this->playerCam.Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.2f, 1000.f, XM_PI * 0.5f, position + cameraOffset, cameraForward);
}

Player::~Player()
{
}

void Player::Update(const float& deltaTime)
{
	this->playerCam.SetTransform(this->playerModel->GetModelMatrix());
	this->playerCam.Update();
}

void Player::Move(float x, float z)
{
	DirectX::SimpleMath::Matrix rotation = {};
	rotation = rotation.CreateFromYawPitchRoll(this->playerModel->GetRotation().y, this->playerModel->GetRotation().x, this->playerModel->GetRotation().z);
	Vector3 pos = this->playerModel->GetPosition();
	Vector3 newPos = pos;
	newPos += Vector3::Transform({ x, 0.0f, z }, rotation);
	//if (newPos.y > 0.00000f)
	//{
	//	newPos.y = 0.0000f;
	//}
	//if (newPos.y < 0.00000f)
	//{
	//	newPos.y = 0.0000f;
	//}
	this->playerModel->SetPosition(newPos);
	this->playerModel->UpdateMatrix();
}

void Player::Rotate(const float& pitch, const float& yaw)
{
	float pitchX = this->playerModel->GetRotation().x;
	float yawY = this->playerModel->GetRotation().y;
	float rollZ = this->playerModel->GetRotation().z;

	float newPitchX = fmod(pitchX + pitch, FULL_CIRCLE);
	float newYawY = fmod(yawY + yaw, FULL_CIRCLE);
	this->playerModel->SetRotation({ newPitchX, newYawY, rollZ });
	this->playerModel->UpdateMatrix();
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}