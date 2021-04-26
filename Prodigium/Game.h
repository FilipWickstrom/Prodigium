#pragma once
#include "Engine.h"
#include "CameraObject.h"
class Game:public Engine
{
private:
	/*
		Setups
	*/
	CameraObject tempCam;
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