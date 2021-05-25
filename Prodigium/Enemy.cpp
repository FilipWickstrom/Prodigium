#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;
Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster.obj", "Monster_Albedo.png", "Monster_Normal.jpg", false, false, { 1.0f, 1.0f, 1.0f });
	this->attackRange = 20.f;
	this->speed = 40.f;
	this->reachedTarget = false;
	this->model->forward = { 0.0f, 0.0f, 1.0f };
	this->model->forward.Normalize();
	this->model->up = { 0.0f, 1.0f, 0.0f };
	this->model->up.Normalize();
	this->model->rotation = { 0.0f, 1.0f, 0.0f };
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
	//Vector3 source = this->model->forward;
	this->model->forward = targetPos - this->model->position;
	this->model->forward.Normalize();
	//Vector3 dest = this->model->forward;
	//this->model->right = this->model->up.Cross(this->model->forward);

	//float dot = source.Dot(dest);

	/*Matrix rotation = Matrix::CreateRotationY(dot);

	this->model->rotation = Vector3::Transform(Vector3(0.0f, 0.0f, 0.0f), rotation);*/

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


