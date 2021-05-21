#pragma once
#include "Engine.h"
#include "CameraObject.h"
#include "Player.h"
#include "Enemy.h"
#include "MainMenu.h"
#include <thread>
#include <omp.h>
#include "RandomSpotPicker.h"

class Game:public Engine
{
private:
	/*
		Setups
	*/
	CameraObject tempCam;
	MainMenu menu;
	Player* player;
	Enemy* enemy;
	RandomSpotPicker picker;
	bool running;
	bool hasLoaded;
	bool zoomIn;
	bool inGoal;
	bool isPaused;
public:
	Game(const HINSTANCE& instance, const UINT& windowWidth, const UINT& windowHeight);
	~Game();
	const bool IsRunning() const;
	void HandleInput(const float& deltaTime);
	// Inherited via Engine
	virtual bool OnFrame(const float& deltaTime) override;
	virtual bool OnStart() override;
	void LoadMap();
	void LoadMainMenu();

	// Put in here whatever values that need reset after each game.
	void ResetValues();
};