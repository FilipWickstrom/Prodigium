#pragma once
#include "GameObject.h"
#include <iostream>
#include <SimpleMath.h>
constexpr float ROTATION_SPEED = 4.f;
constexpr float FULL_CIRCLE = 360.f;
class CameraObject :
	public GameObject
{
private:
	struct viewProjectionMatrix
	{
		DirectX::SimpleMath::Matrix viewMatrix;
		DirectX::SimpleMath::Matrix projectionMatrix;
	};
	const DirectX::SimpleMath::Vector3 defaultUp = { 0.f, 1.f, 0.f };
	const DirectX::SimpleMath::Vector3 defaultForward = { 0.f, 0.f, 1.f };
	viewProjectionMatrix viewProjMatrix;
	DirectX::SimpleMath::Vector3 eyePos;
	DirectX::SimpleMath::Vector3 targetPos;
	DirectX::SimpleMath::Vector3 upDir;
	DirectX::SimpleMath::Matrix rotationMatrix;
	DirectX::SimpleMath::Vector3 rotation;
	float pitch, yaw;
	float fieldOfView;
	float aspectRatio;
	float nearPlane;
	float farPlane;
	ID3D11Buffer* matrixBuffer;
	void UpdateViewProjection();
public:
	CameraObject();
	virtual ~CameraObject();
	bool Initialize(int windowWidth, int windowHeight, float nearPlane, float farPlane, float fov, float aspectRatio, DirectX::SimpleMath::Vector3 eyePosition);
	DirectX::SimpleMath::Matrix GetViewMatrix() const;
	DirectX::SimpleMath::Matrix GetProjectionMatrix() const;
	void Move(float x, float y);
	void Rotate(float pitchAmount, float yawAmount);
	void SetPosition(float xPos, float yPos);
	ID3D11Buffer*& GetViewProjMatrix();
};

