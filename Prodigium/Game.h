#pragma once
#include "Engine.h"
#include "CameraObject.h"
#include "Player.h"

class Game:public Engine
{
private:
	/*
		Setups
	*/
	CameraObject tempCam;
	Player* player;
	bool running;
public:
	Game(HINSTANCE& instance, UINT windowWidth, UINT windowHeight);
	~Game();
	bool IsRunning() const;
	void HandleInput(float deltaTime);
	// Inherited via Engine
	virtual bool OnFrame(const float& deltaTime) override;
	virtual bool OnStart() override;
};