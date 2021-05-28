#include "Game.h"
#include <iostream>
#include <thread>
#include "ParticleSystem.h"
#include "GUIHandler.h"
#include <omp.h>

#define EDITSCENE SceneHandler()->EditScene()

DirectX::SimpleMath::Vector2 direction(0.0f, 0.0f);

void Game::Whisper()
{
	if (player->GetSanity() > 0)
	{
		int whisperFactor = player->GetSanity() * 100;
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
		//std::cout << "Current Monster Sound Timer: " << this->monsterSoundTimer << "\r";
	}
}

void Game::HandleScenes(const float& deltaTime)
{
	if (this->zoomIn)
	{
		this->menu.ZoomIn({ 30.0f, -22.0f, 63.0f, 1.0f }, deltaTime, this->inGoal);
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
		Engine::isPaused = false;
		this->soundHandler.ResumeAudio();
		GUIHandler::ShowGameGUI(true);
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
		
		player->Update(EDITSCENE.GetAllStaticObjects(), direction, deltaTime);
		GUIHandler::SetPlayerPos(player->GetPlayerPos());
		if (!this->isPaused)
		{
			Whisper(); //Checks every frame if you should get a whisper, and then randomize which one you should get
			BulletTime(); //Slows down all sounds if you're near the enemy
			MonsterSounds(deltaTime); //Monster makes a sound every 5 seconds, that plays in 3D space
		}

		//Different things happen at level of sanity
		int sanity = player->GetSanity();
		int maxSanity = player->GetMaxSanity();

		//100% - 60%: noblur
		if (sanity <= maxSanity && sanity > (maxSanity * 0.6f))
		{
			this->player->SetBlurLevel(BlurLevel::NOBLUR);
		}
		//60% - 40%: easy blur
		else if (sanity <= (maxSanity * 0.6f) && sanity > (maxSanity * 0.4f))
		{
			this->player->SetBlurLevel(BlurLevel::LOW);
		}
		//40% - 20%: medium blur
		else if (sanity <= (maxSanity * 0.4f) && sanity > (maxSanity * 0.2f))
		{
			this->player->SetBlurLevel(BlurLevel::MEDIUM);
		}
		//20% - 0%: hard blur
		else if (sanity <= (maxSanity * 0.2f) && sanity > 0)
		{
			this->player->SetBlurLevel(BlurLevel::HIGH);
		}
		//When player is dead
		if (sanity <= 0)
		{
			this->player->SetSanity(0);
			this->player->SetMovement(false);
			this->player->GetMeshObject()->ChangeAnimState(AnimationState::DEAD);
			this->soundHandler.PlayOneShot(0);
			this->player->SetBlurLevel(BlurLevel::HELLAHIGH);
		}

		if (this->attackTimer > 0 && !this->isPaused)
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
		for (int i = 2; i < EDITSCENE.GetNrOfDynamicObjects(); i++)
		{
			float dist;
			if (dist = EDITSCENE.GetDynamicObject(1).GetDistance(EDITSCENE.GetDynamicObject(i)) < 15.0f && EDITSCENE.GetDynamicObject(i).IsVisible())
			{
				if (this->typeOfTrap[index] == 0)
				{
					this->enemy->SetSpeedFactor(0.0f);
				}
				else
				{
					this->enemy->SetSpeedFactor(0.1f);
				}
				EDITSCENE.GetDynamicObject(i).SetVisible(false);
			}
			index++;
		}
	}	

	if (!this->isPaused && !this->menu.IsInMenu())
	{
		this->soundHandler.Update(this->player->GetPlayerPos(), this->enemy->GetMeshObject()->position, this->player->GetMeshObject()->forward, this->enemy->GetMeshObject()->forward);
		AIHandler::MoveEnemy(deltaTime);
	}

	Engine::isPaused = this->isPaused;
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
		GUIHandler::ShowGameGUI(false);
		this->isPaused = true;
		Engine::isPaused = true;
		this->soundHandler.SuspendAudio();
	}

	// Resume the game.
	else if (!GUIHandler::InOptionsMenu() && this->hasLoaded && this->isPaused && InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		GUIHandler::ResumeGame();
		GUIHandler::ShowGameGUI(true);
		this->isPaused = false;
		Engine::isPaused = false;
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
	}

	if (InputHandler::IsKeyPressed(Keyboard::Escape) && this->hasLoaded && options.state == 2)
	{
		GUIHandler::ShowInGameOptionsMenu(false);
	}

	// You collected all clues! You are WIN!!
	if ( this->player && this->player->GetCollectedClues() >= (this->options.difficulty * 2))
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
			this->player->SetSanity(0);
		}
		if (InputHandler::IsKeyPressed(Keyboard::H))
		{
			this->player->SetSanity(50);
		}
		if (InputHandler::IsKeyPressed(Keyboard::J))
		{
			this->player->SetSanity(100);
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
			//Sideways
			if (InputHandler::IsKeyHeld(Keyboard::A))
			{
				direction.y = -1.f;
				this->player->GetMeshObject()->ChangeAnimState(AnimationState::LEFTSTRAFE);
			}
			else if (InputHandler::IsKeyHeld(Keyboard::D))
			{
				direction.y = 1.f;
				this->player->GetMeshObject()->ChangeAnimState(AnimationState::RIGHTSTRAFE);
			}

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

			//When any of movementkeys is released go back to idle animation
			else if (InputHandler::IsKeyReleased(Keyboard::W) || 
					 InputHandler::IsKeyReleased(Keyboard::S) ||
					 InputHandler::IsKeyReleased(Keyboard::A) ||
					 InputHandler::IsKeyReleased(Keyboard::D) )
			{
				//Randomize idle state
				AnimationState state;
				if (rand() % 4 == 0)
					state = AnimationState::IDLE2;
				else
					state = AnimationState::IDLE;
				this->player->GetMeshObject()->ChangeAnimState(state);
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
					SceneHandler()->EditScene().GetMeshObject(i).SetVisible(false);
					this->player->IncreaseCollectedClues();
					this->player->IncreaseSanity((int)(25 / (this->options.difficulty * 0.5)));
				}
			}
		}
		if (InputHandler::IsRMBPressed())
		{
			if (GUIHandler::ActiveTrap() && this->stopcompl_timer <= 0.0f)
			{
				EDITSCENE.AddDynamicObject("trap_bear.obj", "BearTrapAlbedo.png", "", true, false,
					{ this->player->GetMeshObject()->GetPosition().x, -5.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
					{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }, // Rotation
					{ 0.6f, 0.6f, 0.6f });
				this->typeOfTrap.push_back(0);
				this->stopcompl_timer = STOPCOOLDOWN * this->options.difficulty;
			}
			else if (!GUIHandler::ActiveTrap() && this->slowdown_timer <= 0.0f)
			{
				EDITSCENE.AddDynamicObject("trap_barbwire.obj", "BarbWireTrapAlbedo.png", "", true, false,
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
	Engine::Render(this->player);

	if (!this->isPaused && player)
	{
		AIHandler::MoveEnemy(deltaTime);
		// So we don't go over a certain value
		player->SetCollectedClues(std::min(player->GetCollectedClues(), options.difficulty * 2));

		if (this->slowdown_timer > 0.0f)
		{
			this->slowdown_timer -= 1.0f * deltaTime;
			this->slowdown_timer = std::max(this->slowdown_timer, 0.0f);
		}

		if (this->stopcompl_timer > 0.0f)
		{
			this->stopcompl_timer -= 1.0f * deltaTime;
			this->stopcompl_timer = std::max(this->stopcompl_timer, 0.0f);
		}
	}


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
	this->player->SetSanity(this->player->GetMaxSanity());
	this->player->SetCollectedClues(0);
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
	AIHandler::Remove();
	if (this->player)
		delete this->player;

	if (this->enemy)
		delete this->enemy;

	options.state = MAINMENU;
	// Refresh the game to a clean slate.
	SceneHandler()->RemoveAllScenes();
	SceneHandler()->AddScene();

	int randX = rand() % 80 - rand() % 80;
	int randZ = rand() % 60 + 10;
	EDITSCENE.Add("trap_barbwire.obj", "BarbWireTrapAlbedo.png", "", false, false, { 0.0f, -100.0f, 0.0f });

	//Static objects
	EDITSCENE.Add("geo_bedroom.obj", "Bedroom_Diffuse.png", "Bedroom_Normal.png", false, false, { 0.0f, -30.0f, 30.0f }, { 0.f, 1.57f, 0.0f }, {1.5f, 1.5f, 1.5f});
	EDITSCENE.Add("clue_toy.obj", "toy_albedo.png", "", false, false, {-22.0f, -30.0f, 80.0f}, {0.0f, 0.78f, 0.0f}, {0.5f, 0.5f, 0.5f});
	EDITSCENE.Add("clue_mask.obj", "mask_albedo.png", "mask_normal.png", false, false, {-30.0f, -3.0f, 80.0f}, {-0.52f, 3.14f, 0.0f}, {1.0f, 1.0f, 1.0f});
	EDITSCENE.Add("clue_book.obj", "book_albedo.png", "", false, false, {42.0f, -22.0f, 58.0f}, {0.0f, 2.35f, 0.0f}, {0.5f, 0.5f, 0.5f});
	EDITSCENE.Add("clue_necklace.obj", "necklace_albedo.png", "", false, false, {44.0f, -22.0f, 58.0f}, {1.9f, 0.0f, 0.0f}, {1.5f, 1.5f, 1.5f});
	EDITSCENE.Add("text_prodigium.obj", "ProdigiumTextAlbedo.png", "", false, false, { 0.0f, 5.0f, 20.0f }, { 0.0f, 0.0f, 0.0f }, {0.5f, 0.5f, 0.5f});

	//Animated Objects
	EDITSCENE.Add("Player", "Char_Albedo.png", "Char_Normal.jpg", false, true, { 35.0f, -24.0f, 70.0f }, {0.0f, 1.57f, 0.0f}, {2.0f, 2.0f, 2.0f});
	EDITSCENE.GetMeshObject(EDITSCENE.GetNumberOfObjects() - 1).ChangeAnimState(AnimationState::DEAD);
	EDITSCENE.Add("Monster", "monster_albedo.png", "Monster_Normal.jpg", false, true, { -35.0f, -23.0f, 80.0f }, { 0.0f, 0.0f, 0.0f });
	
	EDITSCENE.Add("geo_bedroom.obj", "Bedroom_Diffuse.png", "Bedroom_Normal.png", false, false, { 0.0f, 30.0f, 30.0f }, { 0.f, 1.57f, XM_PI }, { 3.0f, 3.0f, 3.0f });

	//Varde Ljus
	LightStruct L;
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { 20.0f, 0.0f, 20.0f, 35.0f };
	EDITSCENE.AddLight(L);
	L.direction = { 0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { -20.0f, 0.0f, 20.0f, 35.0f };
	EDITSCENE.AddLight(L);

	ToggleSSAO(false);
	EDITSCENE.GetParticles().SetActive(false);

	this->hasLoaded = false;
	this->inGoal = false;
}

void Game::LoadMap()
{
	options.state = INGAME;

	SceneHandler()->AddScene();
	
	//Add player with the standard idle animation from start
	this->player = new Player();
	Engine::EDITSCENE.AddDynamicObject(this->player->GetMeshObject());
	this->player->GetMeshObject()->ChangeAnimState(AnimationState::IDLE);

	//Enemy
	this->enemy = new Enemy();
	Engine::EDITSCENE.AddDynamicObject(this->enemy->GetMeshObject());
	AIHandler::Initialize();
	AIHandler::SetEnemyAndPlayer(enemy, player);
	//this->enemy->GetMeshObject()->position = { 10.f, 0.f, 10.f };
	// 
	// Terrain
	SceneHandler()->EditScene().Add("geo_terrain.obj", "Terrain_Diffuse.png", "Terrain_Normal.png", false, false, { 0.0f, -5.25f, 0.0f }, { 0.0f, 0.0f, 0.0f },
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

		EDITSCENE.Add("clue_" + clue + ".obj", clue + "_albedo.png", normalmap, false, false,
			{ pos.x, -3.0f, pos.y }, rotation, scale);

		L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
		L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
		L.position = { pos.x, 0.0f, pos.y , 5.0f };
		EDITSCENE.AddLight(L);
	}

	// Houses around the town.
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, -50.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, -50.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 225.0f, -7.0f, 125.0f }, { 0.0f, 4.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, 200.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, 200.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 330.0f, -7.0f, 100.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 450.0f, -7.0f, 100.0f }, { 0.0f, 4.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 500.0f, -7.0f, -10.0f }, { 0.0f, 4.71238898f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 200.0f, -7.0f, -100.0f }, { 0.0f, 1.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 175.0f, -7.0f, -350.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 50.0f, -7.0f, -350.0f }, { 0.0f, 0.0f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 100.0f, -7.0f, -135.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -5.0f, -7.0f, -135.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 325.0f, -7.0f, 225.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 425.0f, -7.0f, 225.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 675.0f, -7.0f, -25.0f }, { 0.0f, 4.71f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 675.0f, -7.0f, 100.0f }, { 0.0f, 4.71f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 575.0f, -7.0f, 175.0f }, { 0.0f, 3.14159f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -200.0f, -7.0f, -200.0f }, { 0.0f, 1.57f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -200.0f, -7.0f, -330.0f }, { 0.0f, 1.57f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 125.0f, -7.0f, -240.0f }, { 0.0f, 1.57f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { 75.0f, -7.0f, -240.0f }, { 0.0f, 4.7123f, 0.0f });
	EDITSCENE.Add("geo_house1.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, false, { -275.0f, -7.0f, 125.0f }, { 0.0f, 3.14159f, 0.0f });

	/*
		Lamps
	*/
	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 575.0f, -7.0f, 50.0f }, { 0.0f, 1.57f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 575.0f, 25.0f, 50.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { -45.0f, -7.0f, -100.0f }, { 0.0f, 1.57f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { -45.0f, 25.0f, -100.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 130.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 130.0f, 25.0f, 0.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 30.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 0.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 190.0f, -7.0f, 90.0f }, { 0.0f, 4.14159f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 190.0f, 25.0f, 90.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 30.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 100.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 130.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 130.0f, 25.0f, 100.0f, 30.0f };
	EDITSCENE.AddLight(L);

	EDITSCENE.Add("geo_lamp1.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, false, { 325.0f, -7.0f, -75.0f }, { 0.0f, 1.57079633f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 325.0f, 25.0f, -75.0f, 30.0f };
	EDITSCENE.AddLight(L);

	// Big momma house
	EDITSCENE.Add("geo_house2.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", true, false, { -150.0f, 1.0f, 50.0f }, { 0.0f, 4.71238898f, 0.0f }, { 3.0f, 3.0f, 3.0f });
	EDITSCENE.Add("geo_house2.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", true, false, { 475.0f, 1.0f, -250.0f }, { 0.0f, 2.35619449f, 0.0f }, { 4.0f, 4.0f, 4.0f });
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

		EDITSCENE.Add("shitty_tree.obj", "puke_color.png", "", false, false, { x, -5.5f, z }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
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

		EDITSCENE.Add("shitty_mountain.obj", "gray_color.png", "", true, false, { x, -12.5f, z }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f, 10.0f });
	}


	this->amountOfObjects = EDITSCENE.GetNumberOfObjects();
	Engine::inGame = true;
	Engine::quadTree = new QuadTree;
	Engine::quadTree->BuildQuadTree(EDITSCENE.GetAllMeshObjects());
	ToggleSSAO(true);

	this->inGoal = true;
}