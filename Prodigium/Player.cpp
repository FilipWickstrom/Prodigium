#include "Player.h"
#include "Graphics.h"
#include "ResourceManager.h"

using namespace DirectX::SimpleMath;

void Player::RotatePlayer()
{
	Vector3 currentRotation = { 0.f, this->playerModel->rotation.y, 0.f };
	Vector3 targetRotation = { 0.f, this->playerCam->GetRotation().y + DirectX::XM_PI, 0.f };

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
}

Player::Player()
{
	this->playerModel = new MeshObject();
	this->playerModel->Initialize("Player", "Char_Albedo.png", "Char_Normal.jpg", true, true, { 0,-5,0 }, {0, DirectX::XM_PI,0});

	Vector3 cameraOffset = { 0.0f, 7.5f, -20.f };
	Vector3 cameraForward = this->playerModel->colliders[0].boundingBox.Center - cameraOffset;
	cameraForward.y += 9;
	cameraForward.Normalize();
	this->speed = 10.f;
	this->sanity = 100.f;
	this->health = 100;
	this->cluesCollected = 0;
	this->playerModel->forward = { 0.0f, 0.0f, 1.0f };
	this->playerModel->right = this->playerModel->up.Cross(this->playerModel->forward);
	this->playerCam = new CameraObject;
	ResourceManager::AddCamera("PlayerCam", playerCam);
	this->playerCam->Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.1f, 425.f, DirectX::XM_PI * 0.5f, cameraOffset, cameraForward);
	
	//Fixes the large hitbox of the tposing player
	this->playerModel->collidersOriginal[0].boundingBox.Extents.x = this->playerModel->collidersOriginal[0].boundingBox.Extents.x / 3.f;
	this->playerModel->colliders[0].boundingBox.Extents.x = this->playerModel->colliders[0].boundingBox.Extents.x / 3.f;

	// Force update to rotate to correct direction of player
	this->playerModel->UpdateMatrix();
	this->playerModel->UpdateBoundingBoxes();
}

Player::~Player()
{
	ResourceManager::RemoveCamera("PlayerCam");
}

void Player::Update(const std::vector<MeshObject*>& objects, DirectX::SimpleMath::Vector2& direction, const float& deltaTime)
{
	if (direction.Length() > 0.0f)
	{
		if (!CheckCollision(objects, direction, deltaTime))
		{
			this->Move(direction, deltaTime);
		}
	}
	Matrix transform = Matrix::CreateTranslation(this->playerModel->position);
	this->playerCam->SetTransform(transform);
	this->playerCam->Update();
}

void Player::Move(Vector2& direction, const float& deltaTime)
{
	Matrix rotation = Matrix::CreateRotationY(this->playerCam->rotation.y);
	this->playerModel->forward = Vector3::TransformNormal(Vector3(0.0f, 0.0f, 1.0f), rotation);
	this->playerModel->right = this->playerModel->up.Cross(this->playerModel->forward);

	this->playerModel->position +=
		(this->playerModel->forward * speed * deltaTime * direction.x) +
		(this->playerModel->right * speed * deltaTime * direction.y);

	// Moves character in the direction of camera
	this->RotatePlayer();
	this->playerModel->UpdateMatrix();
	this->playerModel->UpdateBoundingBoxes();
}

void Player::RotateCamera(const float& pitch, const float& yaw)
{
	this->playerCam->Rotate(pitch, yaw, 0.f);
}

void Player::Sprint()
{
	this->speed = 35.0f;
}

void Player::SetSanity(const float& newSanity)
{
	this->sanity = newSanity;
}

void Player::IncreaseSanity(const float& amount)
{
	this->sanity += amount;
}

const float& Player::GetSanity() const
{
	return this->sanity;
}

void Player::SetHealth(const int& newHealth)
{
	this->health = newHealth;
}

void Player::IncreaseHealth(const int& amount)
{
	this->health = std::min(this->health + amount, 100);
	this->sanity = this->health * 0.01f;
}

const int& Player::GetHealth() const
{
	return this->health;
}

void Player::SetCollectedClues(const int& newCollected)
{
	this->cluesCollected = newCollected;
}

void Player::IncreaseCollectedClues()
{
	this->cluesCollected++;
}

const int& Player::GetCollectedClues() const
{
	return this->cluesCollected;
}

const float& Player::GetSpeed() const
{
	return this->speed;
}

void Player::Walk()
{
	this->speed = 20.0f;
}

const DirectX::SimpleMath::Vector3& Player::GetPlayerPos()
{
	return this->playerModel->GetPosition();
}

MeshObject* Player::GetMeshObject() const
{
	return this->playerModel;
}

void Player::TakeDamage(const float& damage)
{

}

bool Player::CheckCollision(const std::vector<MeshObject*>& objects, const Vector2& direction, const float& deltaTime)
{
	for (int i = 1; i < (int)objects.size(); i++)
	{
		for (int j = 0; j < objects[i]->colliders.size(); j++)
		{
			if (this->playerModel->colliders[0].boundingBox.Intersects(objects[i]->colliders[j].boundingBox))
			{
				// Project the u vector onto the plane normal to get a length down to the player position
				// Take that length - the halflength of current OBB to get the difference. 
				// If the difference is positive and it's the smallest of all sides, we know the colliding plane
				Vector3 u = this->playerModel->position - objects[i]->position;

				float lastDistance = FLT_MAX;
				int index = 0;
				Vector3 halfLengths = objects[i]->colliders[j].boundingBox.Extents;
				for (int k = 0; k < 4; k++)
				{
					Vector3 n = objects[i]->colliders[j].planes[k].normal;
					float dot = u.Dot(n);
					float currentDistance = 0.0f;

					if (dot < 0.0000f)
					{
						continue;
					}
					float projectedLength = (dot * n).Length();

					if (k == 0 || k == 1)
					{
						currentDistance = projectedLength - halfLengths.z;
					}
					else
					{
						currentDistance = projectedLength - halfLengths.x;
					}

					if (currentDistance < lastDistance && currentDistance > 0.000f)
					{
						index = k;
						lastDistance = currentDistance;
					}
				}
				this->playerModel->position += objects[i]->colliders[j].planes[index].normal * speed * deltaTime;
				this->playerModel->UpdateMatrix();
				this->playerModel->UpdateBoundingBoxes();

				return true;
			}
		}
	}

	return false;
}
