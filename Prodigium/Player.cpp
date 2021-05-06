#include "Player.h"
using namespace DirectX::SimpleMath;

Player::Player()
{
	Vector3 position(0.0f, 0.0f, 0.f);
	Vector3 cameraOffset = { 0.0f, 2.5f, -15.f };
	Vector3 cameraForward = cameraOffset * -1;
	cameraForward.Normalize();
	this->speed = 10.f;
	this->playerModel = new MeshObject();
	this->playerModel->Initialize("LowPoly_Character.obj", "Char_Albedo.png");
	this->playerModel->forward = { 0.0f, 0.0f, 1.0f };
	this->playerModel->right = this->playerModel->up.Cross(this->playerModel->forward);
	this->playerModel->rotation = { 0.f, DirectX::XM_PI, 0.f };
	this->playerModel->position = { 0.0f, 0.0f, 0.0f };
	this->playerCam.Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.2f, 1000.f, DirectX::XM_PI * 0.5f, cameraOffset, cameraForward);

	// Force update to rotate to correct direction of player
	this->playerModel->UpdateMatrix();
}

Player::~Player()
{
}

void Player::Update(const float& deltaTime)
{
	DirectX::SimpleMath::Matrix transform = DirectX::SimpleMath::Matrix::CreateTranslation(this->playerModel->position);
	this->playerCam.SetTransform(transform);
	this->playerCam.Update();
}

void Player::Move(const Vector2& direction, const float& deltaTime)
{
	DirectX::SimpleMath::Matrix rotation = DirectX::SimpleMath::Matrix::CreateRotationY(this->playerCam.GetRotation().y);
	this->playerModel->forward = Vector3::Transform(Vector3(0.0f, 0.0f, 1.0f), rotation);
	this->playerModel->forward.Normalize();
	this->playerModel->right = this->playerModel->up.Cross(this->playerModel->forward);
	this->playerModel->right.Normalize();

	// Direction.x / z is a binary switch to toggle the direction, 1 or -1
	this->playerModel->position += this->playerModel->forward * speed * deltaTime * direction.x;
	this->playerModel->position += this->playerModel->right * speed * deltaTime * direction.y;

	Vector3 currentRotation = { 0.f, this->playerModel->rotation.y, 0.f };
	Vector3 targetRotation = { 0.f, this->playerCam.GetRotation().y + DirectX::XM_PI, 0.f };

	// Not so elegant way of half-fixing the lerp issue
	if ((currentRotation.y - targetRotation.y) > 4.5f)
	{
		currentRotation.y -= FULL_CIRCLE;
	}
	else if ((targetRotation.y - currentRotation.y) > 4.5f)
	{
		currentRotation.y += FULL_CIRCLE;
	}

	this->playerModel->rotation.y = Vector3::Lerp(currentRotation, targetRotation, 0.1f).y;

	this->playerModel->UpdateMatrix();
}

void Player::Rotate(const float& pitch, const float& yaw)
{
	this->playerCam.Rotate(pitch, yaw, 0.f);
}

void Player::Sprint()
{
	this->speed = 50.0f;
}

void Player::Walk()
{
	this->speed = 20.0f;
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}