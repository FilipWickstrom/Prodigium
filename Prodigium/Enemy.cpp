#include "Enemy.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

Enemy::Enemy()
{
	this->model = new MeshObject();
	this->model->Initialize("Monster.obj", "Monster_Albedo.png", "Monster_Normal.jpg", false, false, { 1.0f, 1.0f, 1.0f });
	this->attackRange = 20.f;
	this->speed = 30.f;
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

	float x = model->forward.Dot(this->targetDir);
	//float theta = acos(x);
	//float y = sin(theta);
	float y = sqrt(1.0f - (x * x));
	float tempAngle = atan2f(y, x);

	if (isnan(tempAngle))
	{
		tempAngle = 0.f;
	}
	tempAngle = acos(x);

	this->angle = fmod(this->angle + tempAngle, FULL_CIRCLE);
}

void Enemy::MoveToTarget(const float& deltaTime)
{
	Vector3 current = { 0.0f, this->model->rotation.y, 0.0f };
	Vector3 target = { 0.0f, this->angle, 0.0f };
    this->model->rotation.y = Vector3::Lerp(current, target, speed * deltaTime).y;

	printf("%1.6f\r", this->model->rotation.y);

	this->model->forward = Vector3::Lerp(this->model->forward, this->targetDir, speed * deltaTime);
	this->model->forward.Normalize();
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


