#include "Player.h"
using namespace DirectX::SimpleMath;

Player::Player()
{
	Vector3 position(0.0f, 0.0f, 0.f);
	Vector3 cameraOffset = { 0.0f, 2.5f, -8.f };
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
	// Moves the player in the direction of the mouse and lerps between current player rotation
	// and the cameras rotation to avoid snapping effects.

	// Position
	direction.Normalize();
	direction = direction * speed * deltaTime;
	DirectX::SimpleMath::Matrix rotation = {};
	rotation = rotation.CreateFromYawPitchRoll(this->playerCam.GetRotation().y, 0.f, 0.f);
	Vector3 currentPos = this->playerModel->GetPosition();
	Vector3 newPos = Vector3::Transform({ direction }, rotation);
	newPos += currentPos;
	this->playerModel->SetPosition(newPos);

	// Rotation
	Vector3 currentRotation = { 0.f, this->playerModel->GetRotation().y, 0.f };
	Vector3 targetRotation = { 0.f, this->playerCam.GetRotation().y + DirectX::XM_PI, 0.f };

	if (abs(targetRotation.y - currentRotation.y) > 0.1f)
	{
		currentRotation.y = targetRotation.y;
	}

	targetRotation = Vector3::Lerp(currentRotation, targetRotation, 0.1f);
	this->playerModel->SetRotation({ 0, targetRotation.y, 0 });
	this->playerModel->UpdateMatrix();
}

void Player::Rotate(const float& pitch, const float& yaw)
{
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

const DirectX::SimpleMath::Vector3 Player::getPlayerPos()
{
	return this->playerModel->GetPosition();
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}