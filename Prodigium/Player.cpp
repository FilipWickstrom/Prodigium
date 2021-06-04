#include "Player.h"
#include "Graphics.h"
#include "ResourceManager.h"

using namespace DirectX::SimpleMath;

void Player::RotatePlayer()
{
	Quaternion currentRot = Quaternion::CreateFromYawPitchRoll(this->playerModel->rotation.y, 0.f, 0.f);
	Quaternion targetRot = Quaternion::CreateFromYawPitchRoll(this->playerCam->GetRotation().y + DirectX::XM_PI, 0.f, 0.f);

	this->playerModel->qRotation = Quaternion::Slerp(this->playerModel->qRotation, targetRot, 0.2f);
}

Player::Player()
{
	this->playerModel = new MeshObject;
	this->playerModel->Initialize("Player", true, true, { -40.f,-5.3f,45.f }, { 0, -DirectX::XM_PIDIV2,0 });

	Vector3 cameraOffset = { 0.0f, 7.5f, -17.f };
	this->speed = 10.f;
	this->maxSanity = 100;
	this->sanity = this->maxSanity;
	this->cluesCollected = 0;
	this->playerModel->forward = { 0.0f, 0.0f, 1.0f };
	this->playerModel->right = this->playerModel->up.Cross(this->playerModel->forward);
	this->playerCam = new CameraObject;
	ResourceManager::AddCamera("PlayerCam", playerCam);
	this->playerCam->Initialize(Graphics::GetWindowWidth(), Graphics::GetWindowHeight(), 0.1f, 425.f, DirectX::XM_PI * 0.5f, cameraOffset, { 0.0f, 0.0f, 1.0f });
	this->playerCam->MoveCameraTowards(this->playerModel->position);

	//Fixes the large hitbox of the tposing player
	this->playerModel->collidersOriginal[0].boundingBox.Extents.x = this->playerModel->collidersOriginal[0].boundingBox.Extents.x / 3.f;
	this->playerModel->colliders[0].boundingBox.Extents.x = this->playerModel->colliders[0].boundingBox.Extents.x / 3.f;

	// Force update to rotate to correct direction of player
	this->playerModel->UpdateMatrix();
	this->playerModel->UpdateBoundingBoxes();

	this->moving = true;
}

Player::~Player()
{
	ResourceManager::RemoveCamera("PlayerCam");
}

void Player::Update(const std::unordered_map<std::uintptr_t, MeshObject*>& objects, DirectX::SimpleMath::Vector2& direction)
{
	if (direction.Length() > 0.0f)
	{
		this->Move(direction);
	}
	CheckCollision(objects);
	this->playerCam->MoveCameraTowards(this->playerModel->position);
	this->playerCam->Update();
}

void Player::Move(Vector2& direction)
{
	if (this->moving)
	{
		Matrix rotation = Matrix::CreateRotationY(this->playerCam->rotation.y);
		this->playerModel->forward = Vector3::TransformNormal(Vector3(0.0f, 0.0f, 1.0f), rotation);
		this->playerModel->right = this->playerModel->up.Cross(this->playerModel->forward);

		Vector3 currentDirection = this->playerModel->forward * direction.x + this->playerModel->right * direction.y;
		currentDirection.Normalize();

		this->playerModel->position += currentDirection * this->speed * Graphics::deltaTime;

		// Moves character in the direction of camera
		this->RotatePlayer();
		this->playerModel->UpdateByQuaternion(this->playerModel->qRotation);
		this->playerModel->UpdateBoundingBoxes();
	}
}

void Player::RotateCamera(const float& pitch, const float& yaw)
{
	this->playerCam->Rotate(pitch, yaw, 0.f);
}

void Player::Sprint()
{
	this->speed = 35.0f;
}

void Player::SetSanity(const int& newSanity)
{
	this->sanity = newSanity;
}

void Player::IncreaseSanity(const int& amount)
{
	this->sanity = std::min(this->sanity + amount, this->maxSanity);
}

const int& Player::GetSanity() const
{
	return this->sanity;
}

const int& Player::GetMaxSanity() const
{
	return this->maxSanity;
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

//void Player::CheckCollision(const std::unordered_map<std::uintptr_t, MeshObject*>& objects, const float& deltaTime)
//{
//	for (auto object : objects)
//	{
//		for (int i = 0; i < object.second->colliders.size(); i++)
//		{
//			DirectX::ContainmentType type = this->playerModel->colliders[0].boundingBox.Contains(object.second->colliders[i].boundingBox);
//			if (type == DirectX::ContainmentType::DISJOINT)
//			{
//				continue;
//			}
//			// Project the u vector onto the plane normal to get a length down to the player position
//			// Take that length (-+) the halflength of current OBB to get the difference. 
//			// If the difference is positive and it's the smallest of all sides, we which of the 
//			// planes of the OBB we collided which. That will move the player in the direction
//			// of the planes normal.
//			Vector3 u = this->playerModel->colliders[0].boundingBox.Center - object.second->colliders[i].boundingBox.Center;
//			Vector3 corners[8];
//			this->playerModel->colliders[0].boundingBox.GetCorners(corners);
//			Vector3 halfLengths = object.second->colliders[i].boundingBox.Extents;
//
//			float lastDistance = FLT_MAX;
//			int index = 0;
//
//			for (int k = 0; k < 4; k++)
//			{
//				Vector3 n = object.second->colliders[i].planes[k].normal;
//				float dot = u.Dot(n);
//				float currentDistance = 0.0f;
//
//				// Current plane normal is on the opposite side, continue.
//				if (dot < 0.0000f)
//				{
//					continue;
//				}
//				float projectedLength = (dot * n).Length();
//
//				if (k == 0 || k == 1)
//				{
//					currentDistance = projectedLength - halfLengths.z;
//				}
//				else
//				{
//					currentDistance = projectedLength - halfLengths.x;
//				}
//
//				if (abs(currentDistance) < lastDistance)
//				{
//					index = k;
//					lastDistance = abs(currentDistance);
//				}
//			}
//			// Take a vector from the planes point to every corner of the players boundingbox
//			// Project that vector onto the normal of the plane we are working on (based on the smallest)
//			// dot product we found and take the length of that vector. This will give the EXACT length
//			// of how much we need to push the player back.
//
//			float currentDot = FLT_MAX;
//			Vector3 planePoint = object.second->colliders[i].planes[index].point;
//			Vector3 planeNormal = object.second->colliders[i].planes[index].normal;
//			for (int j = 0; j < 8; j++)
//			{
//				float dot = (corners[j] - planePoint).Dot(planeNormal);
//				currentDot = std::min(dot, currentDot);
//			}
//			float length = (currentDot * object.second->colliders[i].planes[index].normal).Length();
//			this->playerModel->position += planeNormal * length;
//			this->playerModel->UpdateByQuaternion(this->playerModel->qRotation);
//			this->playerModel->UpdateBoundingBoxes();
//		}
//	}
//}

void Player::CheckCollision(const std::unordered_map<std::uintptr_t, MeshObject*>& objects)
{
	for (auto object : objects)
	{
		for (int i = 0; i < object.second->colliders.size(); i++)
		{
			DirectX::ContainmentType type = this->playerModel->colliders[0].boundingBox.Contains(object.second->colliders[i].boundingBox);
			if (type == DirectX::ContainmentType::DISJOINT)
			{
				continue;
			}

			Vector3 corners[8];
			this->playerModel->colliders[0].boundingBox.GetCorners(corners);
			Vector3 halfLengths = object.second->colliders[i].boundingBox.Extents;

			float lastDistance = FLT_MAX;
			int planeIndex = 0;
			int cornerIndex = 0;

			// Loop through all planes of the colliding object to see which direction
			// to push the player towards
			for (int k = 0; k < 4; k++)
			{
				Vector3 planeNormal = object.second->colliders[i].planes[k].normal;
				Vector3 planePoint = object.second->colliders[i].planes[k].point;
				// Normal points in the opposite direction, no need to check this plane
				if ((this->playerModel->colliders[0].boundingBox.Center - object.second->colliders[i].boundingBox.Center).Dot(planeNormal) < 0.000f)
				{
					continue;
				}

				float previousLength = FLT_MAX;
				float currentLength = 0.0f;
				// Get the corner of the playerCollider that is furthest inside the OBB
				for (int j = 0; j < 8; j++)
				{
					float currentLength = (corners[j] - object.second->colliders[i].boundingBox.Center).Length();
					if (currentLength < previousLength)
					{
						previousLength = currentLength;
						cornerIndex = j;
					}
				}

				Vector3 u = corners[cornerIndex] - planePoint;
				float dot = u.Dot(planeNormal);
				currentLength = 0.0f;

				float projectedLength = (dot * planeNormal).Length();
				currentLength = projectedLength;

				if (currentLength < lastDistance)
				{
					planeIndex = k;
					lastDistance = currentLength;
				}
			}
			// Take a vector from the planes point to every corner of the players boundingbox
			// Project that vector onto the normal of the plane we are working on (based on the smallest)
			// dot product we found and take the length of that vector. This will give the EXACT length
			// of how much we need to push the player back.

			float currentDot = FLT_MAX;
			Vector3 planePoint = object.second->colliders[i].planes[planeIndex].point;
			Vector3 planeNormal = object.second->colliders[i].planes[planeIndex].normal;
			for (int j = 0; j < 8; j++)
			{
				float dot = (corners[j] - planePoint).Dot(planeNormal);
				currentDot = std::min(dot, currentDot);
			}
			float length = (currentDot * object.second->colliders[i].planes[planeIndex].normal).Length();
	
			this->playerModel->position += planeNormal * length;
			this->playerModel->UpdateByQuaternion(this->playerModel->qRotation);
			this->playerModel->UpdateBoundingBoxes();
		}
	}
}

void Player::SetMovement(bool toggle)
{
	this->moving = toggle;
}

bool Player::IsMoving()
{
	return this->moving;
}
