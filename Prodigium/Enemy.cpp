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

	this->targetDir = this->targetPos - this->model->position;
	this->targetDir.Normalize();
}

void Enemy::MoveToTarget(const float& deltaTime)
{
	float angle = acos(this->model->forward.Dot(targetDir));
	Quaternion rot = Quaternion::CreateFromAxisAngle(Vector3::Up, angle);
	this->model->forward = Vector3::Transform(Vector3(0.0f, 0.0f, -1.0f), rot);
	this->model->rotation.y += angle * speed * deltaTime;
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


