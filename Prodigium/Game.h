#pragma once
#include "Engine.h"

class Game:public Engine
{
private:
	/*
		Setups
	*/
	bool running;
public:
	Game(HINSTANCE& instance, UINT windowWidth, UINT windowHeight);
	~Game();
	bool IsRunning() const;
	void HandleInput();
	// Inherited via Engine
	virtual bool OnFrame(const float& deltaTime) override;
	virtual bool OnStart() override;
};