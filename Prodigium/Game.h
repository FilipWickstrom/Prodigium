#pragma once
#include "Engine.h"
#include "CameraObject.h"
#include "Player.h"
#include <thread>
#include <omp.h>

#include "AnimatedObject.h"		//DELETE LATER***

class Game:public Engine
{
private:
	/*
		Setups
	*/
	CameraObject tempCam;
	Player* player;
	bool running;
	//float deltaTime, lastFrame, currentFrame;

	AnimatedObject animObj;		//DELETE LATER***

public:
	Game(const HINSTANCE& instance, const UINT& windowWidth, const UINT& windowHeight);
	~Game();
	const bool IsRunning() const;
	void HandleInput(const float& deltaTime);
	// Inherited via Engine
	virtual bool OnFrame(const float& deltaTime) override;
	virtual bool OnStart() override;
	void LoadMap();
};