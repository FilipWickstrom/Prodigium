#pragma once
#include "MeshObject.h"
#include "CameraObject.h"
#include "BlurFilter.h"

class Player
{
private:
	int sanity;
	int maxSanity;
	int cluesCollected; // clues
	float speed;
	CameraObject* playerCam;
	MeshObject* playerModel;
	bool moving;
	BlurLevel currentBlurLevel;

private:
	void RotatePlayer();

public:
	Player();
	virtual ~Player();

	void Update(const std::unordered_map<std::uintptr_t, MeshObject*>& objects, DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void Move(DirectX::SimpleMath::Vector2& direction, const float& deltaTime);
	void RotateCamera(const float& pitch, const float& yaw);
	void Sprint();

	//Sanity
	void SetSanity(const int& newSanity);
	void IncreaseSanity(const int& amount);
	const int& GetSanity() const;
	const int& GetMaxSanity() const;

	//Clues
	void SetCollectedClues(const int& newCollected);
	void IncreaseCollectedClues();
	const int& GetCollectedClues() const;
	
	//Movement
	const float& GetSpeed() const;
	void Walk();

	const DirectX::SimpleMath::Vector3& GetPlayerPos();
	MeshObject* GetMeshObject() const;

	void CheckCollision(const std::unordered_map<std::uintptr_t, MeshObject*>& objects, const DirectX::SimpleMath::Vector2& direction, const float& deltaTime);

	void SetMovement(bool toggle);
	bool IsMoving();

	const BlurLevel& GetBlurLevel() const;
	void SetBlurLevel(BlurLevel level);

};