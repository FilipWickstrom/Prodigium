#include "Player.h"

using namespace DirectX::SimpleMath;

Player::Player()
{
	this->speed = 5.f;
	this->playerCam.Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.2f, 1000.f, XM_PI * 0.5f, { 0.0f, 0.0f, -10.f });
	this->playerModel = new MeshObject();
	this->playerModel->Initialize("cube.obj", "", "", { 0.0f, 0.0f, -4.f });
}

Player::~Player()
{
}

void Player::Update(const float& deltaTime)
{
	this->playerCam.Update();
	this->playerCam.Bind();
}

void Player::Move(float x, float z)
{
	DirectX::SimpleMath::Matrix rotation = {};
	rotation = rotation.CreateFromYawPitchRoll(this->playerModel->GetRotation().y, this->playerModel->GetRotation().x, this->playerModel->GetRotation().z);
	Vector3 pos = this->playerModel->GetPosition();
	Vector3 newPos = pos;
	//Vector3 newPos = pos.Transform({ x, 0.0f, z }, rotation);
	newPos += pos.Transform({ x, 0.0f, z }, rotation);
	this->playerModel->SetPosition(newPos);
	this->playerModel->UpdateMatrix();
	
	float deltaX = newPos.x - pos.x;
	float deltaY = newPos.y - pos.y;
	float deltaZ = newPos.z - pos.z;

	// TODO MAKE CAMERA FOLLOW PLAYER
	this->playerCam.Move(deltaX, deltaY, deltaZ);
}

void Player::Rotate(const float& pitch, const float& yaw)
{

	float pitchX = this->playerModel->GetRotation().x;
	float yawY = this->playerModel->GetRotation().y;
	float rollZ = this->playerModel->GetRotation().z;

	pitchX = fmod(pitchX + pitch, FULL_CIRCLE);
	yawY = fmod(yawY + yaw, FULL_CIRCLE);
	this->playerModel->SetRotation({ pitchX, yawY, rollZ });
	this->playerModel->UpdateMatrix();
	this->playerCam.Rotate(pitch, yaw);
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}