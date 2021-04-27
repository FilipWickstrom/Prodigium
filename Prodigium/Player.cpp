#include "Player.h"

Player::Player()
{
	this->speed = 5.f;
	this->BuildMatrix({ 0.0f, 0.0f, -25.f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
	this->playerCam.Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.2f, 1000.f, XM_PI * 0.5f, { 0.0f, 0.0f, 2.f });
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
	this->playerCam.Move(x, z);
}

void Player::Rotate(const float& pitch, const float& yaw)
{
	this->playerCam.Rotate(pitch, yaw);
}
