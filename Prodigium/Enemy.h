#pragma once
#include "MeshObject.h"
#include "Player.h"
#include <omp.h>
class Enemy
{
private:
	MeshObject* model;
	float speed;
	float attackRange;
	DirectX::SimpleMath::Vector3 targetPos;
	const DirectX::SimpleMath::Vector3 defaultForward = { 0.f,0.f,1.f };
	float lastAttack;
	bool reachedTarget;
public:
	Enemy();
	virtual ~Enemy();
	void SetNewTarget(const DirectX::SimpleMath::Vector3& newPos);
	void Move(const DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void MoveToTarget(const float& deltaTime);
	const float& GetAttackRange() const;
	void PlayAttackAnimation();
	const bool& CanAttack() const;
	void Attack(Player* player);
	const DirectX::SimpleMath::Vector3& getPosition() const;
	const bool IsCloseToPlayer(const DirectX::SimpleMath::Vector3& playerPos);
	MeshObject* GetMeshObject() const;
	const bool& HasReachedTarget() const;


};

