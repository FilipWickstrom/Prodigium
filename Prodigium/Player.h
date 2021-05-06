#pragma once
#include "CameraObject.h"
#include "Graphics.h"
#include "MeshObject.h"

class Player
{
private:
	float speed;
	CameraObject playerCam;
	MeshObject* playerModel;

public:
	Player();
	virtual ~Player();

	void Update(const float& deltaTime);
	void Move(const DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void Rotate(const float& pitch, const float& yaw);
	void Sprint();
	void Walk();
	const DirectX::SimpleMath::Vector3 getPlayerPos();
	MeshObject* GetMeshObject()const;
	bool CheckCollision(MeshObject* other);
};