#include "Enemy.h"
#include "UsefulHeader.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy()
{
	this->model = new MeshObject;
	this->model->Initialize("Monster", "monster_Albedo.png", "Monster_Normal.jpg", true, true, { 1.0f, -3.0f, 1.0f });
	this->attackRange = 20.f;
	this->speed = 10.f;
	this->reachedTarget = false;
	this->model->forward = { 0.0f, 0.0f, -1.0f };
	this->model->forward.Normalize();
	this->model->up = { 0.0f, 1.0f, 0.0f };
	this->model->up.Normalize();
	this->angle = 0.0f;
	this->speedFactor = 1.0f;
	this->speedDegradeCooldown = 0;
	this->model->qRotation = Quaternion::Identity;

	this->model->collidersOriginal[0].boundingBox.Extents.x /= 3.f;
	this->lastAttack = 0;
}

Enemy::~Enemy()
{
}

void Enemy::SetNewTarget(const Vector3& newPos)
{
	this->targetPos = newPos;
	this->reachedTarget = false;

	this->targetDir = this->targetPos - this->model->position;
	this->targetDir.y = 0.0f;
	this->targetDir.Normalize();

	float theta = asin(this->targetDir.x);

	if (this->targetDir.z > 0.0f)
	{
		if (this->targetDir.x > 0.0f)
		{
			theta = XM_PI - theta;
		}
		else
		{
			theta = -XM_PI - theta;
		}
	}

	this->angle = -theta;
}

void Enemy::Chase(const DirectX::SimpleMath::Vector3& playerPos, const float& deltaTime)
{
	const float ROTATION_SPEED = 5.f;

	// Update speed factor.
	this->speedDegradeCooldown = std::max(this->speedDegradeCooldown, 0.0f);
	if (this->speedDegradeCooldown > 0)
		this->speedDegradeCooldown -= 1 * deltaTime;
	if (this->speedDegradeCooldown < 0)
		this->speedFactor = 1.0f;

	this->model->forward = playerPos - this->model->position;
	this->model->forward.Normalize();

	Quaternion q2 = Quaternion::CreateFromAxisAngle(this->model->up, this->angle);
	this->model->qRotation = Quaternion::Slerp(this->model->qRotation, q2, ROTATION_SPEED * deltaTime);

	this->model->position += this->model->forward * this->speed * this->speedFactor * deltaTime;
	this->model->UpdateByQuaternion(this->model->qRotation);
	this->model->UpdateBoundingBoxes();
}

void Enemy::MoveToTarget(const float& deltaTime)
{
	const float ROTATION_SPEED = 5.f;

	// Update speed factor.
	this->speedDegradeCooldown = std::max(this->speedDegradeCooldown, 0.0f);
	if (this->speedDegradeCooldown > 0)
		this->speedDegradeCooldown -= 1 * deltaTime;
	if (this->speedDegradeCooldown < 0)
		this->speedFactor = 1.0f;

	this->model->forward = Vector3::Lerp(this->model->forward, this->targetDir, this->speed * deltaTime);
	this->model->forward.Normalize();

	Quaternion q2 = Quaternion::CreateFromAxisAngle(this->model->up, this->angle);
	this->model->qRotation = Quaternion::Slerp(this->model->qRotation, q2, ROTATION_SPEED * deltaTime);

	this->model->position += this->model->forward * this->speed * this->speedFactor * deltaTime;

	if ((this->model->position - targetPos).Length() < 10.f)
	{
		reachedTarget = true;
	}

	this->model->UpdateByQuaternion(this->model->qRotation);
	this->model->UpdateBoundingBoxes();
}

const float& Enemy::GetAttackRange() const
{
	return this->attackRange;
}

void Enemy::PlayAttackAnimation()
{
}

const bool Enemy::CanAttack() const
{
	return (omp_get_wtime() - lastAttack > 3);
}

void Enemy::Attack(Player* player)
{
	this->lastAttack = omp_get_wtime();
	player->IncreaseSanity(-20);
}

const DirectX::SimpleMath::Vector3& Enemy::GetPosition() const
{
	return this->model->position;
}

const bool Enemy::IsCloseToPlayer(const DirectX::SimpleMath::Vector3& playerPos)
{
	bool toReturn = false;
	if ((this->model->position - playerPos).Length() < 50.f)
	{
		toReturn = true;
	}
	return toReturn;
}

MeshObject* Enemy::GetMeshObject() const
{
	return this->model;
}

const bool& Enemy::HasReachedTarget() const
{
	return this->reachedTarget;
}

void Enemy::SetSpeedFactor(float factor)
{
	if (this->speedDegradeCooldown <= 0)
	{
		this->speedFactor = factor;
		this->speedDegradeCooldown = 5.0f;
	}
}

void Enemy::RotateTo(const Vector3& targetDirection)
{
	float theta = asin(targetDirection.x);

	if (targetDirection.z > 0.0f)
	{
		if (targetDirection.x > 0.0f)
		{
			theta = XM_PI - theta;
		}
		else
		{
			theta = (-XM_PI - theta);
			theta += XM_2PI;
		}
	}
	else
	{
		if (targetDirection.x < 0.0f)
		{
			theta += XM_2PI;
		}
	}

	this->angle = -theta;
}