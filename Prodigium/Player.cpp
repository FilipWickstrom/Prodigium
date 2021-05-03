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
	// Moves the player in the direction of the mouse and lerps between current rotation and a target rotation
	// to avoid snapping effects.
	direction.Normalize();
	direction = direction * speed * deltaTime;
	DirectX::SimpleMath::Matrix rotation = {};
	rotation = rotation.CreateFromYawPitchRoll(this->playerCam.GetRotation().y, 0.f, 0.f);
	Vector3 currentPos = this->playerModel->GetPosition();
	Vector3 newPos = Vector3::Transform({ direction }, rotation);
	newPos += currentPos;

	Vector3 newRotation = this->playerModel->GetRotation().Lerp({ 0.f, this->playerModel->GetRotation().y, 0.f }, { 0.f, this->playerCam.GetRotation().y + DirectX::XM_PI, 0.f }, 0.1f);
	this->playerModel->SetRotation({ 0, newRotation.y, 0 });
	this->playerModel->SetPosition(newPos);
	this->playerModel->UpdateMatrix();
}

void Player::Rotate(const float& pitch, const float& yaw)
{
	// Free look camera
	float yawY = this->playerModel->GetRotation().y;
	float pitchX = this->playerModel->GetRotation().x;

	float newPitchX = fmod(pitchX + pitch, FULL_CIRCLE);
	float newYawY = fmod(yawY + yaw, FULL_CIRCLE);

	this->playerCam.Rotate(pitch, yaw, 0);
	//std::cout << this->playerCam.GetRotation().y << std::endl;
}

void Player::Sprint()
{
	this->speed = 50.0f;
}

void Player::Walk()
{
	this->speed = 20.0f;
}

void Player::ChangeCameraOffset(const DirectX::SimpleMath::Vector3& change)
{
	this->cameraOffset += change;
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}