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
	//Vector3 offset;
	DirectX::SimpleMath::Vector3 cameraDistance;
	DirectX::SimpleMath::Vector3 forwardVector;
	const DirectX::SimpleMath::Vector3 defaultForward = { 0.f, 0.f, 1.f };
	DirectX::SimpleMath::Vector3 cameraOffset;

public:
	Player();
	virtual ~Player();

	void Update(const float& deltaTime);
	void Move(DirectX::SimpleMath::Vector3& direction, const float& deltaTime);
	void Rotate(const float& pitch, const float& yaw);
	void Sprint();
	void Walk();
	void ChangeCameraOffset(const DirectX::SimpleMath::Vector3& change);
	MeshObject* GetMeshObject()const;
};