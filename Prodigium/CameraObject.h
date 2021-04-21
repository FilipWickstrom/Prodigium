#pragma once
#include "GameObject.h"
#include <iostream>
#include "SimpleMath.h"

	constexpr float ROTATION_SPEED = 4.f;
	constexpr float FULL_CIRCLE = 360.f;
class CameraObject :
	public GameObject
{
private:
	struct viewProjectionMatrix
	{
		XMMATRIX viewMatrix;
		XMMATRIX projectionMatrix;
	};
	const XMVECTOR defaultUp = { 0.f, 1.f, 0.f, 0.f };
	const XMVECTOR defaultForward = { 0.f, 0.f, 1.f, 0.f }; 
	viewProjectionMatrix viewProjMatrix;
	XMVECTOR eyePos;
	XMVECTOR targetPos;
	XMVECTOR upDir;
	XMMATRIX rotationMatrix;
	XMVECTOR rotation;
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
	bool Initialize(ID3D11Device*& device, int windowWidth, int windowHeight, float nearPlane, float farPlane, float fov, float aspectRatio, XMVECTOR& eyePosition);
	XMMATRIX GetViewMatrix() const;
	XMMATRIX GetProjectionMatrix() const;
	void Move(float x, float y);
	void Rotate(float pitchAmount, float yawAmount);
	void setPosition(float xPos, float yPos);
};

