#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;
Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster.obj", "Monster_Albedo.png", "Monster_Normal.jpg", false);
	this->attackRange = 20.f;
	this->speed = 40.f;
	this->reachedTarget = false;
	this->model->forward = { 0.0f, 0.0f, 1.0f };
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
	Vector3 newForward = targetPos - this->model->position;
	newForward.Normalize();
	//this->model->position = Vector3::Lerp(this->model->position, targetPos, 0.2f * deltaTime);
	Matrix rotation = Matrix::CreateFromYawPitchRoll(newForward.y, 0.0f, 0.0f);

	this->model->forward = Vector3::TransformNormal(Vector3(0.0f, 0.0f, 1.0f), rotation);
	this->model->position += this->model->forward * 10.f * deltaTime;

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


