#pragma once
#include "Engine.h"
#include "CameraObject.h"
#include "Player.h"
#include "Enemy.h"
#include "MainMenu.h"
#include <thread>
#include "SoundHandler.h"
#include "RandomSpotPicker.h"

// will also be affected by difficulty set in options, refer to "OptionsHandler".
#define STOPCOOLDOWN 12.5f
#define SLOWCOOLDOWN 5.0f
#define ENEMY_ATTACK_DAMAGE 5
#define ENEMY_ATTACK_COOLDOWN 3.5f
#define ENEMY_ATTACK_RANGE 10.0f

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
	SoundHandler soundHandler;
	RandomSpotPicker picker;
	bool running;
	bool hasLoaded;
	bool zoomIn;
	bool inGoal;

	bool isInOptions;
	std::vector<int> trapIndices;
	float attackTimer;

	void Whisper();
	void BulletTime();
	void HandleScenes(const float& deltaTime);
	void HandleGameLogic(const float& deltaTime);
	int amountOfObjects;
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