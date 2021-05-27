#pragma once
#include "MeshObject.h"
#include "CameraObject.h"

class Player
{
private:
	float sanity; // Blur
	int health; // Health
	int cluesCollected; // clues
	float speed;
	CameraObject* playerCam;
	MeshObject* playerModel;

private:
	void RotatePlayer();

public:
	Player();
	virtual ~Player();

	void Update(const std::vector<MeshObject*>& objects, DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void Move(DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void RotateCamera(const float& pitch, const float& yaw);
	void Sprint();
	void SetSanity(const float& newSanity);
	void IncreaseSanity(const float& amount);
	const float& GetSanity() const;
	void SetHealth(const int& newHealth);
	void IncreaseHealth(const int& amount);
	const int& GetHealth() const;
	void SetCollectedClues(const int& newCollected);
	void IncreaseCollectedClues();
	const int& GetCollectedClues() const;
	const float& GetSpeed() const;
	void Walk();
	const DirectX::SimpleMath::Vector3& GetPlayerPos();
	MeshObject* GetMeshObject() const;
	void TakeDamage(const float& damage);
	bool CheckCollision(const std::vector<MeshObject*>& objects, const DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
};