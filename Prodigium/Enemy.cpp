#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;
Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster.obj", "Monster_Albedo.png", "Monster_Normal.jpg", false);
	this->attackRange = 20.f;
	this->speed = 0.5f;
	this->reachedTarget = false;
}

Enemy::~Enemy()
{
}

void Enemy::SetNewTarget(const DirectX::SimpleMath::Vector3& newPos)
{
	this->targetPos = newPos;
	this->reachedTarget = false;
}

void Enemy::Move(const DirectX::SimpleMath::Vector2& direction, const float& deltaTime)
{

	this->model->position += {direction.x* speed* deltaTime, 0.f, direction.y* speed* deltaTime};
	this->model->UpdateMatrix();
}

void Enemy::MoveToTarget(const float& deltaTime)
{
	this->model->position = Vector3::Lerp(this->model->position, targetPos, 0.2f * deltaTime);
	if ((this->model->position - targetPos).Length() < 10.f)
	{
		reachedTarget = true;
	}

	this->model->UpdateMatrix();
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


