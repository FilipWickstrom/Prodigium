#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;
Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster.obj");
	this->attackRange = 20.f;
	this->speed = 40.f;
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

void Enemy::MoveToTarget(const float& deltaTime)
{
	this->model->position = Vector3::Lerp(this->model->position, targetPos, 2.f * deltaTime);
	if ((this->model->position - targetPos).Length() < 10.f)
	{
		reachedTarget = true;
	}
	this->model->UpdateMatrix();
}

const bool& Enemy::HasReachedTarget() const
{
	return this->reachedTarget;
}


