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
	void Move(float x, float z);
	void Rotate(const float& pitch, const float& yaw);
	MeshObject* GetMeshObject()const;
};