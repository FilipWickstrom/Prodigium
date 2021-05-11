#pragma once
#include "GameObject.h"
#include <iostream>
#include <SimpleMath.h>
constexpr float ROTATION_SPEED = 4.f;
constexpr float FULL_CIRCLE = 6.283185f;
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
	DirectX::SimpleMath::Vector3 defaultForward;
	DirectX::SimpleMath::Vector3 defaultPosition;
	viewProjectionMatrix viewProjMatrix;
	DirectX::SimpleMath::Vector3 eyePos;
	DirectX::SimpleMath::Vector4 eyePosGPU;
	DirectX::SimpleMath::Vector3 upDir;
	DirectX::SimpleMath::Vector3 camForward;
	float pitch, yaw, roll;
	float fieldOfView;
	float aspectRatio;
	float nearPlane;
	float farPlane;
	ID3D11Buffer* matrixBuffer;
	ID3D11Buffer* camPosBuffer;
	void UpdateViewMatrix();
public:
	CameraObject();
	virtual ~CameraObject();
	void SetTransform(const DirectX::SimpleMath::Matrix& transform);
	bool Initialize(const int& windowWidth,const int& windowHeight, const float& nearPlane,const float& farPlane,const float& fov, const DirectX::SimpleMath::Vector3& eyePosition, const DirectX::SimpleMath::Vector3& lookTo);
	void Rotate(const float& pitchAmount, const float& yawAmount, const float& rollAmount);
	DirectX::SimpleMath::Vector3 GetPos() const;
	void Update();
	//void SetTransform(const DirectX::SimpleMath::Matrix& transform, const DirectX::SimpleMath::Vector3& playerPos);
	void ChangeOffset(const DirectX::SimpleMath::Vector3& offset);
	DirectX::SimpleMath::Vector3 GetForward();
	bool FullRevolution();
};