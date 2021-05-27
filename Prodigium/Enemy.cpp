#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster.obj", "Monster_Albedo.png", "Monster_Normal.jpg", false, false, { 1.0f, 1.0f, 1.0f });
	this->attackRange = 20.f;
	this->speed = 20.f;
	this->reachedTarget = false;
	this->model->forward = { 0.0f, 0.0f, -1.0f };
	this->model->forward.Normalize();
	this->model->up = { 0.0f, 1.0f, 0.0f };
	this->model->up.Normalize();
	this->angle = 0.0f;
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

void Enemy::MoveToTarget(const float& deltaTime)
{
	this->model->forward = Vector3::Lerp(this->model->forward, this->targetDir, 25.f * deltaTime);
	this->model->forward.Normalize();

	Vector3 currentRot = { 0.0f, this->model->rotation.y, 0.0f };
	Vector3 targetRot = { 0.0f, this->angle, 0.0f };

	this->model->rotation.y = Vector3::Lerp(currentRot, targetRot, 5.f * deltaTime).y;

	this->model->position += this->model->forward * this->speed * deltaTime;

	if ((this->model->position - targetPos).Length() < 10.f)
	{
		reachedTarget = true;
	}

	this->model->UpdateMatrix();
}

MeshObject* Enemy::GetMeshObject() const
{
	return this->model;
}

const bool& Enemy::HasReachedTarget() const
{
	return this->reachedTarget;
}


