#pragma once
#include "MeshObject.h"
#include "CameraObject.h"

class Player
{
private:
	float speed;
	CameraObject* playerCam;
	MeshObject* playerModel;
	bool moving;

private:
	void RotatePlayer();

public:
	Player();
	virtual ~Player();

	void Update(const std::unordered_map<std::uintptr_t, MeshObject*>& objects, DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void Move(DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void RotateCamera(const float& pitch, const float& yaw);
	void Sprint();
	void Walk();
	const DirectX::SimpleMath::Vector3& GetPlayerPos();
	MeshObject* GetMeshObject() const;

	bool CheckCollision(const std::unordered_map<std::uintptr_t, MeshObject*>& objects, const DirectX::SimpleMath::Vector2& direction, const float& deltaTime);

	void SetMovement(bool toggle);
	bool IsMoving();
};