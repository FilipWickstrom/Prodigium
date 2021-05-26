#include "Game.h"
#include <iostream>
#include <thread>
#include "ParticleSystem.h"
#include "GUIHandler.h"
#include <omp.h>

#define EDITSCENE SceneHandle()->EditScene()

DirectX::SimpleMath::Vector2 direction(0.0f, 0.0f);

void Game::Whisper()
{
	if (Engine::playerHp > 0)
	{
		int whisperFactor = Engine::playerHp * 100;
		int shouldWhisper = rand() % whisperFactor;

		if (shouldWhisper > 5 && shouldWhisper < 10)
		{
			int index = rand() % 4 + 1;
			this->soundHandler.PlayOneShot(index);
		}
	}
}

void Game::BulletTime()
{
	float distance = DirectX::SimpleMath::Vector4(this->enemy->GetMeshObject()->position - this->player->GetPlayerPos()).Length();
	float factor = std::max(100.0f - distance, 0.0f);
	float speed = factor * 0.01f;
	this->soundHandler.SetPitch(-speed);
}

void Game::MonsterSounds(const float& deltaTime)
{
	if (this->monsterSoundTimer <= 0)
	{
		int index = rand() % 4;
		this->monsterSoundTimer = 5;
		this->soundHandler.PlayMonsterSounds(index);
	}
	else
	{
		this->monsterSoundTimer -= 1 * deltaTime;
		std::cout << "Current Monster Sound Timer: " << this->monsterSoundTimer << "\r";
	}
}

void Game::HandleScenes(const float& deltaTime)
{
	if (this->zoomIn)
	{
		this->menu.ZoomIn({ 0.0f, 15.0f, 100.0f, 1.0f }, deltaTime, this->inGoal);
		GUIHandler::ShowMainMenu(false);
	}
	else if (!this->zoomIn && !this->isInOptions && this->options.state == MAINMENU)
	{
		//Ritar ut Main Menu GUI p? sk?rmen
		GUIHandler::ShowMainMenu(true);
	}

	// Update audio while in options.
	if (this->isInOptions)
	{
		this->soundHandler.SetMusicVolume(options.musicVolume);
		this->soundHandler.SetAmbientVolume(options.ambientVolume);
		this->soundHandler.SetFXVolume(options.sfxVolume);
		this->soundHandler.SetMasterVolume(options.masterVolume);
	}
	if (this->inGoal)
	{
		this->zoomIn = false;
		this->menu.Switch();
	}

	// If inside meny, update with menu buffers.
	if (!this->inGoal)
		this->menu.Update();

	if (!this->menu.IsInMenu() && !this->hasLoaded)
	{
		// Load game map
		this->LoadMap();
		this->hasLoaded = true;
		GUIHandler::ShowGameGUI(true);
		this->soundHandler.PlayAmbient(1);
		this->soundHandler.PlayMusic(0);
	}
	if (this->menu.IsInMenu() && this->hasLoaded)
	{
		// Load menu
		this->LoadMainMenu();
	}

	//Om man trycker p? Resumeknappen i GUI:t ska denna bli true, annars ?r den false
	if (GUIHandler::ShouldResume())
	{
		this->isPaused = false;
		this->soundHandler.ResumeAudio();
	}

	//Om man trycker p? Quitknappen i GUI:t ska denna bli true, annars ?r den false
	if (GUIHandler::ShouldQuit())
		this->running = false;
	if (GUIHandler::InOptionsMenu())
		this->isInOptions = true;
	if (GUIHandler::ShouldReturnToMainMenu())
	{
		this->isPaused = false;
		this->soundHandler.ResumeAudio();
		Engine::inGame = false;
		// Set these values if you want to return to menu.
		this->menu.Switch(true);
		this->ResetValues();
		GUIHandler::ShowMainMenu(true);
		GUIHandler::ShowGameGUI(false);		
	}
}

void Game::HandleGameLogic(const float& deltaTime)
{
	// Return to player buffers.
	if (this->hasLoaded)
	{
		this->options.gameTimer += 1 * deltaTime;
		GUIHandler::ShowMainMenu(false);
		GUIHandler::ShowGameGUI(true);

		player->Update(EDITSCENE.GetAllCullingObjects(), direction, deltaTime);
		GUIHandler::SetPlayerPos(player->GetPlayerPos());
		if (!this->isPaused)
		{
			//Whisper(); //Checks every frame if you should get a whisper, and then randomize which one you should get
			//BulletTime(); //Slows down all sounds if you're near the enemy
			MonsterSounds(deltaTime); //Monster makes a sound every 5 seconds, that plays in 3D space
		}

		if (this->player->GetMeshObject()->GetDistance(SimpleMath::Vector4{ this->enemy->GetMeshObject()->GetPosition().x, this->enemy->GetMeshObject()->GetPosition().y, this->enemy->GetMeshObject()->GetPosition().z , 1.0f }) < ENEMY_ATTACK_RANGE && this->attackTimer <= 0)
		{
			if (Engine::playerHp - (ENEMY_ATTACK_DAMAGE * this->options.difficulty) >= 0)
			{
				Engine::playerHp -= ENEMY_ATTACK_DAMAGE * this->options.difficulty;
			}
			this->attackTimer = ENEMY_ATTACK_COOLDOWN;
		}

		//When player is dead
		if (Engine::playerHp <= 0)
		{
			Engine::playerHp = 0;
			this->player->SetMovement(false);
			this->player->GetMeshObject()->ChangeAnimState(AnimationState::DEAD);
			this->soundHandler.PlayOneShot(0);
		}

		if (this->attackTimer > 0)
		{
			this->attackTimer -= 1 * deltaTime;
		}

		//Check if the current animation has ended
		if (this->player->GetMeshObject()->HasAnimationEnded())
		{
			AnimationState currentState = this->player->GetMeshObject()->GetAnimState();

			//Go from the current state to another when ended
			switch (currentState)
			{
			case AnimationState::PICKUP:
				this->player->SetMovement(true);
				this->player->GetMeshObject()->ChangeAnimState(AnimationState::IDLE);
				break;
			default:
				break;
			};
		}

		// Loops through all traps with monster
		int index = 0;
		for (int i = amountOfObjects; i < EDITSCENE.GetNumberOfObjects(); i++)
		{
			if (EDITSCENE.GetMeshObject(1).GetDistance(EDITSCENE.GetMeshObject(i)) < 5.0f && EDITSCENE.GetMeshObject(i).IsVisible())
			{
				if (this->typeOfTrap[index] == 0)
				{
					this->enemy->SetSpeedFactor(0.0f);
				}
				else
				{
					this->enemy->SetSpeedFactor(0.1f);
				}
				EDITSCENE.GetMeshObject(i).SetVisible(false);
			}
			index++;
		}
	}	

	if (!this->isPaused && !this->menu.IsInMenu())
	{
		this->soundHandler.Update(this->player->GetPlayerPos(), this->enemy->GetMeshObject()->position, this->player->GetMeshObject()->forward, this->enemy->GetMeshObject()->forward);
		//AIHandler::MoveEnemy(deltaTime);
		Engine::Update(deltaTime);
	}
}

Game::Game(const HINSTANCE& instance, const UINT& windowWidth, const UINT& windowHeight)
	:Engine(instance, windowWidth, windowHeight, enemy)
{
	this->running = true;
	this->isPaused = false;
	this->player = nullptr;
	this->hasLoaded = false;
	this->zoomIn = false;
	this->inGoal = false;
	this->amountOfObjects = 0;
	this->attackTimer = 0;
	this->monsterSoundTimer = 0;
	this->isInOptions = false;
}

Game::~Game()
{
	if (this->player && !this->menu.IsInMenu())
		delete this->player;

	if (this->enemy && !this->menu.IsInMenu())
		delete this->enemy;

	AIHandler::Remove();
}

const bool Game::IsRunning() const
{
	if (this->running == false || InputHandler::IsRunning() == false)
	{
		return false;
	}

	return true;
}

void Game::HandleInput(const float& deltaTime)
{
	//Example of how the keyboard and mouse input is gathered and used.
	//Updates the keyboard and mouse with new info about their current state.

	InputHandler::UpdateKeyboardAndMouse();

	direction = { 0.f, 0.f };


	// Pause the game.
	if (!this->isPaused && this->hasLoaded && InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		GUIHandler::PauseGame();
		this->isPaused = true;
		this->soundHandler.SuspendAudio();
	}

	// Resume the game.
	else if (!GUIHandler::InOptionsMenu() && this->hasLoaded && InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		GUIHandler::ResumeGame();
	}

	// Go to Options Menu
	if (InputHandler::IsKeyPressed(Keyboard::P) && !this->hasLoaded)
	{
		this->isInOptions = true;
		GUIHandler::ShowOptionsMenu(true);
		GUIHandler::ShowMainMenu(false);
	}

	// Quit the game while in menu.
	if (InputHandler::IsKeyPressed(Keyboard::Escape) && !this->hasLoaded && !this->isInOptions)
	{
		this->running = false;
	}

	// Return from pause menu.
	if (InputHandler::IsKeyPressed(Keyboard::Escape) && this->isInOptions)
	{
		this->isInOptions = false;
		GUIHandler::ShowOptionsMenu(false);
		GUIHandler::ShowMainMenu(true);
	}

	if (InputHandler::IsKeyPressed(Keyboard::Escape) && this->hasLoaded && options.state == 2)
	{
		GUIHandler::ShowInGameOptionsMenu(false);
	}

	// You collected all clues! You are WIN!!
	if (Engine::cluesCollected >= (this->options.difficulty * 2))
	{
		GUIHandler::ShowMainMenu(true);
		GUIHandler::ShowGameGUI(false);
		Engine::inGame = false;
		// Set these values if you want to return to menu.
		this->menu.Switch(true);
		this->ResetValues();
	}

	// Start the game.
	if (!this->hasLoaded && !this->isInOptions && InputHandler::IsKeyPressed(Keyboard::Space))
	{
		this->zoomIn = true;
	}

	// Go back to Menu
	/*
	if (!this->zoomIn && this->hasLoaded && InputHandler::IsKeyPressed(Keyboard::O))
	{
		Engine::inGame = false;
		// Set these values if you want to return to menu.
		this->menu.Switch(true);
		this->ResetValues();
		GUIHandler::ShowMainMenu(true);
		GUIHandler::ShowGameGUI(false);
	}
	*/

	if (this->hasLoaded && !this->isPaused)
	{
		/*------------------SANITY TESTING----------------*/
		if (InputHandler::IsKeyPressed(Keyboard::G))
		{
			Engine::playerHp = 50;
		}
		if (InputHandler::IsKeyPressed(Keyboard::H))
		{
			Engine::playerHp -= 10;
		}
		if (InputHandler::IsKeyPressed(Keyboard::J))
		{
			Engine::playerHp = 100;
		}
		/*------------------SANITY TESTING----------------*/

		if (InputHandler::IsKeyPressed(Keyboard::K))
		{
			OpenConsole();
		}
		if (InputHandler::IsKeyPressed(Keyboard::L))
		{
			EDITSCENE.GetParticles().SetActive(false);
		}


		/*------------------MOVEMENT----------------*/

		if (this->player->IsMoving())
		{
			//Forward
			if (InputHandler::IsKeyHeld(Keyboard::W))
			{
				//Sprint
				if (InputHandler::IsKeyHeld(Keyboard::LeftShift))
				{
					this->player->Sprint();
					this->player->GetMeshObject()->ChangeAnimState(AnimationState::RUNFORWARD);
				}
				else
				{
					this->player->Walk();
					this->player->GetMeshObject()->ChangeAnimState(AnimationState::WALKFORWARD);
				}
				direction.x = 1.f;
			}
			else if (InputHandler::IsKeyReleased(Keyboard::W))
			{
				//Randomize idle state
				AnimationState state;
				if (rand() % 4 == 0)
					state = AnimationState::IDLE2;
				else
					state = AnimationState::IDLE;
				this->player->GetMeshObject()->ChangeAnimState(state);
			}

			//Backward
			else if (InputHandler::IsKeyHeld(Keyboard::S))
			{
				if (InputHandler::IsKeyHeld(Keyboard::LeftShift))
				{
					this->player->Sprint();
					this->player->GetMeshObject()->ChangeAnimState(AnimationState::RUNBACKWARD);
				}
				else
				{
					this->player->Walk();
					this->player->GetMeshObject()->ChangeAnimState(AnimationState::WALKBACKWARD);
				}
				direction.x = -1.f;
			}
			else if (InputHandler::IsKeyReleased(Keyboard::S))
			{
				//Randomize idle state
				AnimationState state;
				if (rand() % 4 == 0)
					state = AnimationState::IDLE2;
				else
					state = AnimationState::IDLE;
				this->player->GetMeshObject()->ChangeAnimState(state);
			}

			//Sideways
			if (InputHandler::IsKeyHeld(Keyboard::A))
			{
				direction.y = -1.f;
			}
			if (InputHandler::IsKeyHeld(Keyboard::D))
			{
				direction.y = 1.f;
			}
		}

		/*------------------MOVEMENT----------------*/

		/*------Animation settings for player------*/
		if (InputHandler::IsKeyPressed(Keyboard::D9))
		{
			this->player->GetMeshObject()->InterpolateAnim(true);
#ifdef _DEBUG
			std::cout << "[Player] Interpolation: ON" << std::endl;
#endif
		}
		else if (InputHandler::IsKeyPressed(Keyboard::D0))
		{
			this->player->GetMeshObject()->InterpolateAnim(false);
#ifdef _DEBUG
			std::cout << "[Player] Interpolation: OFF" << std::endl;
#endif
		}
		/*------Animation settings for player------*/

		if (InputHandler::IsLMBPressed())
		{
			for (int i = trapIndices[0]; i < trapIndices[0] + (trapIndices.size()); i++)
			{
				if (this->player->GetMeshObject()->GetDistance(EDITSCENE.GetMeshObject(i)) < 5.0f && EDITSCENE.GetMeshObject(i).IsVisible())
				{
					this->player->SetMovement(false);
					this->player->GetMeshObject()->ChangeAnimState(AnimationState::PICKUP);
					SceneHandle()->EditScene().GetMeshObject(i).SetVisible(false);
					Engine::cluesCollected++;
					Engine::playerHp += (int)(25 / (this->options.difficulty * 0.5));
				}
			}
		}
		if (InputHandler::IsRMBPressed())
		{
			if (GUIHandler::ActiveTrap() && this->stopcompl_timer <= 0.0f)
			{
				EDITSCENE.Add("BearTrap_Triangulated.obj", "BearTrapAlbedo.png", "", false, false,
					{ this->player->GetMeshObject()->GetPosition().x, -5.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
					{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }, // Rotation
					{ 0.6f, 0.6f, 0.6f });
				this->typeOfTrap.push_back(0);
				this->stopcompl_timer = STOPCOOLDOWN * this->options.difficulty;
			}
			else if (!GUIHandler::ActiveTrap() && this->slowdown_timer <= 0.0f)
			{
				EDITSCENE.Add("BarbWireTrap_Triangulated.obj", "BarbWireTrapAlbedo.png", "", false, false,
					{ this->player->GetMeshObject()->GetPosition().x, -5.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
					{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }, // Rotation
					{ 1.5f, 1.5f, 1.5f });
				this->typeOfTrap.push_back(1);
				this->slowdown_timer = SLOWCOOLDOWN * this->options.difficulty;
			}
		}
		if (InputHandler::IsKeyPressed(Keyboard::E))
		{
			Engine::ChangeActiveTrap();
		}
		if (InputHandler::getMouseMode() == Mouse::Mode::MODE_RELATIVE && (InputHandler::GetMouseX() != 0 || InputHandler::GetMouseY() != 0))
		{
			int invert = 1;
			if (this->options.inverseSens)
				invert = -1;

			this->player->RotateCamera(invert * InputHandler::GetMouseY() * deltaTime * this->options.mouseSens, invert * InputHandler::GetMouseX() * deltaTime * this->options.mouseSens);
		}


	}
}

bool Game::OnFrame(const float& deltaTime)
{
	// Game loop
	// 1. Handle all inputs
	// 2. Update the game assets and logic
	// 3. Render

	Graphics::SetDeltaTime(deltaTime);
	/*---------------ONE---------------*/
	HandleInput(deltaTime);

	/*---------------TWO---------------*/
	HandleScenes(deltaTime);
	HandleGameLogic(deltaTime);

	/*---------------THREE---------------*/
	Engine::ClearDisplay();
	Engine::Render();

	return true;
}

bool Game::OnStart()
{
#ifdef _DEBUG
	OpenConsole();

	if (!DebugInfo::Initialize())
	{
		return false;
	}
#endif

	this->menu.Init();
	this->LoadMainMenu();

	if (!this->soundHandler.Initialize())
	{
		return false;
	}

	this->soundHandler.SetMasterVolume(options.masterVolume);
	this->soundHandler.SetAmbientVolume(options.ambientVolume);
	this->soundHandler.SetFXVolume(options.sfxVolume);
	this->soundHandler.SetMusicVolume(options.musicVolume);

	this->soundHandler.PlayMusic(1);
	this->soundHandler.PlayAmbient(1);

	return true;
}

void Game::ResetValues()
{
	// Reset values
	Engine::playerHp = 100;
	Engine::playerSanity = 1.0f;
	Engine::cluesCollected = 0;
	this->inGoal = false;
	this->menu.Reset();
	this->picker.Reset();
	Engine::slowdown_timer = 0;
	Engine::stopcompl_timer = 0;
	this->options.gameTimer = 0;
	this->attackTimer = 0;
	this->monsterSoundTimer = 0;
	
	this->soundHandler.SetPitch(0.0f);
	this->soundHandler.PlayMusic(1);
	this->soundHandler.PlayAmbient(1);
}

void Game::LoadMainMenu()
{
	Engine::inGame = false;
	if (this->player)
		delete this->player;

	if (this->enemy)
		delete this->enemy;

	options.state = MAINMENU;

	// Refresh the game to a clean slate.
	SceneHandle()->RemoveAllScenes();
	SceneHandle()->AddScene();
	//EDITSCENE.GetParticles().SetActive(false);

	int randX = rand() % 80 - rand() % 80;
	int randZ = rand() % 60 + 10;

	//Collision not needed in the menu. Set "hasBounds" to false
	SceneHandle()->EditScene().Add("ProdigiumText_TRIANGULATED.obj", "ProdigiumTextAlbedo.png", "", false, false, { 0.0f, 35.0f, 85.0f }, { -0.25f, 0.0f, 0.0f }, {1.5f, 1.5f, 1.5f});
	LightStruct L;
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { 0.0, 40.0f, 60.0f, 35.0f };
	EDITSCENE.AddLight(L);

	//Add player with specific animation
	/*SceneHandler()->EditScene().Add("Player", "Char_Albedo.png", "Char_Normal.jpg", false, true, {0,-5,30.0f});
	SceneHandler()->EditScene().GetMeshObject(SceneHandler()->EditScene().GetNumberOfObjects() - 1).ChangeAnimState(AnimationState::IDLE2);*/
	SceneHandle()->EditScene().Add("Player", "Char_Albedo.png", "Char_Normal.jpg", false, true, {0,-5,80.0f});
	SceneHandle()->EditScene().GetMeshObject(SceneHandle()->EditScene().GetNumberOfObjects() - 1).ChangeAnimState(AnimationState::DEAD);

	//Add animated monster in background
	SceneHandle()->EditScene().Add("Monster", "monster_albedo.png", "Monster_Normal.jpg", false, true, { 60,-5, 110.0f }, { 0,0.5,0 });

	// Terrain
	SceneHandle()->EditScene().Add("tempTerrain.obj", "dirt_color.png", "", false, false, { 0.0f, -6.25f, 0.0f });
	
	// Ominous House
	SceneHandle()->EditScene().Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", false, false, { 0.0f, 0.0f, 150.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 3.0f, 3.0f });
	
	// Directional light
	L.direction = { 0.f, -1.0f, -1.0f, 1.2f };
	L.attentuate = { 0.4f, 0.008f, 0.0f, 0.0f };
	L.position = { 0.0f, 20.0f, 10.0f, 25.0f };
	EDITSCENE.AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", false, false, { -25.0f, -7.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { -25.0, 25.0f, 50.0f, 30.0f };
	EDITSCENE.AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", false, false, { 25.0f, -7.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 25.0, 25.0f, 50.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("BarbWireTrap_Triangulated.obj", "BarbWireTrapAlbedo.png", "", false, false, { 0.0f, -100.0f, 0.0f });

	this->hasLoaded = false;
	this->inGoal = false;
}

void Game::LoadMap()
{
	options.state = INGAME;

	SceneHandle()->AddScene();
	this->player = new Player();
	this->enemy = new Enemy();
	Engine::EDITSCENE.Add(this->player->GetMeshObject());
	Engine::EDITSCENE.Add(this->enemy->GetMeshObject());
	AIHandler::SetEnemy(this->enemy);
	// Terrain
	SceneHandle()->EditScene().Add("planeTerrain.obj", "Terrain_Diffuse.png", "Terrain_Normal.png", false, false, { 0.0f, -5.25f, 0.0f }, { 0.0f, 0.0f, 0.0f },
		{ 1000.0f, 1.0f, 1000.0f });

	LightStruct L;

	// Directional light
	L.direction = { 0.f, -1.0f, -1.0f, 1.2f };
	L.attentuate = { 0.4f, 0.008f, 0.0f, 0.0f };
	L.position = { 0.0f, 20.0f, 10.0f, 25.0f };
	EDITSCENE.AddLight(L);

	for (int i = 0; i < this->options.difficulty * 2; i++)
	{
		trapIndices.push_back(EDITSCENE.GetNumberOfObjects());
		DirectX::SimpleMath::Vector2 pos = this->picker.GetRandomPos();
		std::string clue = this->picker.GetRandomClue();

		SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f };
		SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f };
		std::string normalmap = "";

		// Used to fix inconsistencies in scale and rotation.
		if (clue == "book" || clue == "toy")
			scale = { 0.5f, 0.5f, 0.5f };

		else if (clue == "drawing")
		{
			rotation = { 3.14159f, 3.14159f, 0.0f };
			scale = { 0.5f, 0.5f, 0.5f };
			normalmap = "drawing_normal.png";
		}

		else if (clue == "mask")
		{
			scale = { 1.2f, 1.2f, 1.2f };
			normalmap = "mask_normal.png";
		}
		else if (clue == "knife")
		{
			scale = { 0.25f, 0.25f, 0.25f };
		}

		EDITSCENE.Add(clue + "_OBJ.obj", clue + "_albedo.png", normalmap, false, false,
			{ pos.x, -3.0f, pos.y }, rotation, scale);

		L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
		L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
		L.position = { pos.x, 0.0f, pos.y , 5.0f };
		EDITSCENE.AddLight(L);
	}

	// Houses around the town.
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, -50.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, -50.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 225.0f, -7.0f, 125.0f }, { 0.0f, 4.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, 200.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, 200.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 330.0f, -7.0f, 100.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 450.0f, -7.0f, 100.0f }, { 0.0f, 4.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 500.0f, -7.0f, -10.0f }, { 0.0f, 4.71238898f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 200.0f, -7.0f, -100.0f }, { 0.0f, 1.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 175.0f, -7.0f, -350.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 50.0f, -7.0f, -350.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, -135.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, -135.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 325.0f, -7.0f, 225.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 425.0f, -7.0f, 225.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 675.0f, -7.0f, -25.0f }, { 0.0f, 4.71f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 675.0f, -7.0f, 100.0f }, { 0.0f, 4.71f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 575.0f, -7.0f, 175.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -200.0f, -7.0f, -200.0f }, { 0.0f, 1.57f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -200.0f, -7.0f, -330.0f }, { 0.0f, 1.57f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 125.0f, -7.0f, -240.0f }, { 0.0f, 1.57f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 75.0f, -7.0f, -240.0f }, { 0.0f, 4.7123f, 0.0f });
	EDITSCENE.Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -275.0f, -7.0f, 125.0f }, { 0.0f, 3.14159f, 0.0f });

	/*
		Lamps
	*/
	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 575.0f, -7.0f, 50.0f }, { 0.0f, 1.57f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 575.0f, 25.0f, 50.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { -45.0f, -7.0f, -100.0f }, { 0.0f, 1.57f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { -45.0f, 25.0f, -100.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 130.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 130.0f, 25.0f, 0.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 30.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 0.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 190.0f, -7.0f, 90.0f }, { 0.0f, 4.14159f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 190.0f, 25.0f, 90.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 30.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 100.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 130.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 130.0f, 25.0f, 100.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 325.0f, -7.0f, -75.0f }, { 0.0f, 1.57079633f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 325.0f, 25.0f, -75.0f, 30.0f };
	EDITSCENE.AddLight(L);

	// Big momma house
	EDITSCENE.Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", true, false, { -150.0f, 1.0f, 50.0f }, { 0.0f, 4.71238898f, 0.0f }, { 3.0f, 3.0f, 3.0f });
	EDITSCENE.Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", true, false, { 475.0f, 1.0f, -250.0f }, { 0.0f, 2.35619449f, 0.0f }, { 4.0f, 4.0f, 4.0f });
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { -91.0f, 12.f, 50.0f, 40.0f };
	EDITSCENE.AddLight(L);

	// Big momma house 2
	L.direction = { 0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { 420.0f, 12.f, -195.0f, 40.0f };
	EDITSCENE.AddLight(L);

	// Tree galore!! aka Performance test
	for (int i = 0; i < 1500; i++)
	{
		float x = (float)(rand() % 1500 - rand() % 1500);
		float z = (float)(rand() % 1500 - rand() % 1500);
		while (x > -375 && x < 750 && z > -375 && z < 275)
		{
			x = (float)(rand() % 1500 - rand() % 1500);
			z = (float)(rand() % 1500 - rand() % 1500);
		}

		EDITSCENE.Add("shittytree.obj", "puke_color.png", "", false, false, { x, -5.5f, z }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	}

	for (int i = 0; i < 50; i++)
	{
		float x = (float)(rand() % 1500 - rand() % 1500);
		float z = (float)(rand() % 1500 - rand() % 1500);
		while (x > -375 && x < 750 && z > -375 && z < 275)
		{
			x = (float)(rand() % 1500 - rand() % 1500);
			z = (float)(rand() % 1500 - rand() % 1500);
		}

		EDITSCENE.Add("shittymountain.obj", "gray_color.png", "", true, false, { x, -12.5f, z }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f, 10.0f });
	}


	this->amountOfObjects = EDITSCENE.GetNumberOfObjects();
	Engine::inGame = true;
	Engine::quadTree = new QuadTree;
	Engine::quadTree->BuildQuadTree(EDITSCENE.GetAllMeshObjects());

	this->inGoal = true;
}