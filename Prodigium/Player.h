#pragma once
#include "GameObject.h"
#include "CameraObject.h"
#include "Graphics.h"

class Player:public GameObject
{
private:
	float speed;

public:
	Player();
	virtual ~Player();

	void Update();
	CameraObject playerCam;
};