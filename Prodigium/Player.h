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

private:
	void RotatePlayer();

public:
	Player();
	virtual ~Player();

	void Update(const float& deltaTime);
	void Move(const DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void RotateCamera(const float& pitch, const float& yaw);
	void Sprint();
	void Walk();
	const DirectX::SimpleMath::Vector3& GetPlayerPos();
	MeshObject* GetMeshObject()const;
	bool CheckCollision(const std::vector<MeshObject*>& objects, const DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
};