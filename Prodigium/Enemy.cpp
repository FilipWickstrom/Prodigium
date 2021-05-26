#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster.obj", "Monster_Albedo.png", "Monster_Normal.jpg", false, false, { 1.0f, 1.0f, 1.0f });
	this->attackRange = 20.f;
	this->speed = 10.f;
	this->reachedTarget = false;
	this->model->forward = { 0.0f, 0.0f, -1.0f };
	this->model->forward.Normalize();
	this->model->up = { 0.0f, 1.0f, 0.0f };
	this->model->up.Normalize();
	this->targetDir = this->model->forward;
}

Enemy::~Enemy()
{
}

void Enemy::SetNewTarget(const Vector3& newPos)
{
	this->targetPos = newPos;
	this->reachedTarget = false;

	this->model->forward = this->targetPos - this->model->position;
	this->model->forward.Normalize();
	this->targetDir = this->model->forward;

	//float angle = acos(currentDir.Dot(targetDir));
	//this->model->rotation.y += angle;
}

void Enemy::MoveToTarget(const float& deltaTime)
{
	//this->model->position += this->model->forward * this->speed * deltaTime;

	Vector3 currentDir = this->model->forward;
	targetDir.Normalize();

	float angle = acos(currentDir.Dot(targetDir));
	this->model->rotation.y = angle;

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


