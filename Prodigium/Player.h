#pragma once
#include "CameraObject.h"
#include "Graphics.h"
#include "MeshObject.h"

class Player:public MeshObject
{
private:
	float speed;
	CameraObject playerCam;
	MeshObject* playerModel;

public:
	Player();
	virtual ~Player();

	void Update(const float& deltaTime);
	void Move(DirectX::SimpleMath::Vector3& direction, const float& deltaTime);
	void Rotate(const float& pitch, const float& yaw);
	void Sprint();
	void Walk();
	MeshObject* GetMeshObject()const;
};