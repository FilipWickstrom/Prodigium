#include "Player.h"
using namespace DirectX::SimpleMath;

Player::Player()
{
	Vector3 position(0.0f, 0.0f, 0.f);
	cameraOffset = { 0.0f, 2.5f, -8.f };
	Vector3 cameraForward = position - cameraOffset;
	cameraForward.Normalize();
	this->speed = 10.f;
	this->playerModel = new MeshObject();
	this->playerModel->Initialize("LowPoly_Character.obj", "Char_Normal.jpg", "", position);
	this->playerModel->SetRotation({ 0, DirectX::XM_PI, 0 });
	this->playerModel->UpdateMatrix();
	this->playerCam.Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.2f, 1000.f, DirectX::XM_PI * 0.5f, position + cameraOffset, cameraForward);
}

Player::~Player()
{
}

void Player::Update(const float& deltaTime)
{
	this->playerCam.SetTransform(this->playerModel->GetModelMatrix(), this->playerModel->GetPosition());
	this->playerCam.Update();
}

void Player::Move(Vector3& direction, const float& deltaTime)
{
	direction.Normalize();
	direction = direction * speed * deltaTime * -1;
	DirectX::SimpleMath::Matrix rotation = {};
	rotation = rotation.CreateFromYawPitchRoll(this->playerModel->GetRotation().y, this->playerModel->GetRotation().x, this->playerModel->GetRotation().z);
	Vector3 currentPos = this->playerModel->GetPosition();
	Vector3 newPos = Vector3::Transform({ direction }, rotation);
	newPos += currentPos;

	this->playerModel->SetPosition(newPos);
	this->playerModel->UpdateMatrix();
}

void Player::Rotate(const float& pitch, const float& yaw)
{
	float yawY = this->playerModel->GetRotation().y;
	float pitchX = this->playerModel->GetRotation().x;

	float newPitchX = fmod(pitchX + pitch, FULL_CIRCLE);
	float newYawY = fmod(yawY + yaw, FULL_CIRCLE);
	this->playerModel->SetRotation({ 0, newYawY, 0 });
	this->playerModel->UpdateMatrix();

	this->playerCam.Rotate(pitch, yaw, 0);
}

void Player::Sprint()
{
	this->speed = 50.0f;
}

void Player::Walk()
{
	this->speed = 20.0f;
}

void Player::changeCameraOffset(const DirectX::SimpleMath::Vector3& change)
{
	this->cameraOffset += change;
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}