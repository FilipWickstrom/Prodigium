#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster", "monster_Albedo.png", "Monster_Normal.jpg", false, true, { 1.0f, -3.0f, 1.0f });
	this->attackRange = 20.f;
	this->speed = 20.f;
	this->reachedTarget = false;
	this->model->forward = { 0.0f, 0.0f, -1.0f };
	this->model->forward.Normalize();
	this->model->up = { 0.0f, 1.0f, 0.0f };
	this->model->up.Normalize();
	this->angle = 0.0f;
	this->model->rotation = { 0.0f, 1.0f, 0.0f };
	this->speedFactor = 1.0f;
	this->speedDegradeCooldown = 0;
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

void Enemy::Move(const DirectX::SimpleMath::Vector2& direction, const float& deltaTime)
{

	this->model->position += {direction.x* speed* deltaTime, 0.f, direction.y* speed* deltaTime};
	this->model->UpdateMatrix();
}

void Enemy::MoveToTarget(const float& deltaTime)
{
	// Update speed factor.
	this->speedDegradeCooldown = std::max(this->speedDegradeCooldown, 0.0f);
	if (this->speedDegradeCooldown > 0)
		this->speedDegradeCooldown -= 1 * deltaTime;
	if (this->speedDegradeCooldown < 0)
		this->speedFactor = 1.0f;

	//Vector3 source = this->model->forward;
	this->model->forward = targetPos - this->model->position;
	this->model->forward = Vector3::Lerp(this->model->forward, this->targetDir, 25.f * deltaTime);
	this->model->forward.Normalize();

	Vector3 currentRot = { 0.0f, this->model->rotation.y, 0.0f };
	Vector3 targetRot = { 0.0f, this->angle, 0.0f };

	this->model->rotation.y = Vector3::Lerp(currentRot, targetRot, 5.f * deltaTime).y;

	this->model->position += this->model->forward * (this->speed * this->speedFactor) * deltaTime;

	if ((this->model->position - targetPos).Length() < 10.f)
	{
		reachedTarget = true;
	}

	this->model->UpdateMatrix();
	this->model->UpdateBoundingBoxes();
}

const float& Enemy::GetAttackRange() const
{
	return this->attackRange;
}

void Enemy::PlayAttackAnimation()
{
}

const bool& Enemy::CanAttack() const
{
	return (omp_get_wtime() - lastAttack > 3);
}

void Enemy::Attack(Player* player)
{
	this->lastAttack = omp_get_wtime();
	player->IncreaseHealth(-20.f);
}

const DirectX::SimpleMath::Vector3& Enemy::getPosition() const
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


