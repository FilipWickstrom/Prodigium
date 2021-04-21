#pragma once
#include "Engine.h"

class Game:public Engine
{
private:
	/*
		Setups
	*/

public:
	Game(HINSTANCE& instance, UINT windowWidth, UINT windowHeight);
	~Game();

	// Inherited via Engine
	virtual bool OnFrame(const float& deltaTime) override;
	virtual bool OnStart() override;
};