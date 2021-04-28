#include "Player.h"
using namespace DirectX::SimpleMath;

Player::Player()
{
	this->speed = 5.f;
	this->playerCam.Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.2f, 1000.f, DirectX::XM_PI * 0.5f, { 0.0f, 0.0f, -10.f });
	this->playerModel = new MeshObject();
	this->playerModel->Initialize("cube.obj", "", "", { 0.0f, 0.0f, -4.f });
	this->cameraDistance = { 0.f,0.f,5.f };
	this->forwardVector = { 0.f,0.f,1.f };
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
	//newPos += {x,0.f,z};
	//Vector3 newPos = pos.Transform({ x, 0.0f, z }, rotation);
	newPos += pos.Transform({ x, 0.0f, z }, rotation);
	this->playerModel->SetPosition(newPos);
	this->playerModel->UpdateMatrix();
	//this->cameraDistance = this->cameraDistance.Transform(cameraDistance, rotation);
	Vector3 cameraOffset = playerModel->GetPosition() - cameraDistance;
	cameraOffset = cameraOffset.Transform(cameraOffset, rotation);
	//std::cout << cameraOffset.x << " " << cameraOffset.y << " " << cameraOffset.z << "\r";
	//Vector3 delta = newPos - pos;
	//delta = delta.Transform(delta, rotation);

	// TODO MAKE CAMERA FOLLOW PLAYER
	this->playerCam.SetPosition(cameraOffset);
}

void Player::Rotate(const float& pitch, const float& yaw)
{

	float pitchX = this->playerModel->GetRotation().x;
	float yawY = this->playerModel->GetRotation().y;
	float rollZ = this->playerModel->GetRotation().z;

	pitchX = fmod(pitchX + pitch, DirectX::XM_2PI);
	yawY = fmod(yawY + yaw, DirectX::XM_2PI);
	this->playerModel->SetRotation({ pitchX, yawY, rollZ });
	this->playerModel->UpdateMatrix();
	this->playerCam.SetRotation(rollZ, pitchX, yawY);
	DirectX::SimpleMath::Matrix rotation = DirectX::SimpleMath::Matrix::CreateFromYawPitchRoll(yawY, pitchX, 0);
	std::cout << "Before: " << forwardVector.x << " " << forwardVector.y << " " << forwardVector.z << "\n";

	forwardVector = Vector3::Transform(defaultForward, rotation);
	cameraDistance = -forwardVector;
	cameraDistance.x *= 2.f;
	cameraDistance.y *= 2.f;
	cameraDistance.z *= 2.f;
	std::cout << "After: " << forwardVector.x << " " << forwardVector.y << " " << forwardVector.z << "\n";
	//std::cout << cameraDistance.x << " " << cameraDistance.y << " " << cameraDistance.z << "\n";
	Vector3 cameraOffset = playerModel->GetPosition() - cameraDistance;
	//std::cout << cameraOffset.x << " " << cameraOffset.y << " " << cameraOffset.z << "\r";
	this->playerCam.SetPosition(cameraOffset.Transform(cameraOffset, rotation));
	//this->playerCam.Rotate(pitch, yaw);
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}