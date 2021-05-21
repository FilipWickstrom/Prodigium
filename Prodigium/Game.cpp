#include "Game.h"
#include <iostream>
#include <thread>
#include "ParticleSystem.h"
#include "GUIHandler.h"

DirectX::SimpleMath::Vector2 direction(0.0f, 0.0f);

Game::Game(const HINSTANCE& instance, const UINT& windowWidth, const UINT& windowHeight)
	:Engine(instance, windowWidth, windowHeight, this->enemy)
{
	this->running = true;
	this->isPaused = false;
	this->player = nullptr;
	this->hasLoaded = false;
	this->zoomIn = false;
	this->inGoal = false;
	this->enemy = nullptr;
}

Game::~Game()
{
	if (this->player && !this->menu.IsInMenu())
		delete this->player;
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

	//TODO: Make the engine cleanly shutdown
	if (InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		this->running = false;
	}

	// Start the game.
	if (!this->hasLoaded && InputHandler::IsKeyPressed(Keyboard::Space))
	{
		this->zoomIn = true;
	}

	// Go back to Menu
	if (this->hasLoaded && InputHandler::IsKeyPressed(Keyboard::F10))
	{
		// Set these values if you want to return to menu.
		this->menu.Switch(true);
		this->ResetValues();
		GUIHandler::ShowMainMenu(true);
		GUIHandler::ShowGameGUI(false);
	}

	if (this->hasLoaded && !this->isPaused)
	{
		if (InputHandler::IsKeyPressed(Keyboard::G))
		{
			Engine::playerHp = 50;
		}
		if (InputHandler::IsKeyPressed(Keyboard::H))
		{
			Engine::playerHp = 0;
		}
		if (InputHandler::IsKeyPressed(Keyboard::J))
		{
			Engine::playerHp = 100;
		}

		if (InputHandler::IsKeyPressed(Keyboard::K))
		{
			OpenConsole();
		}
		if (InputHandler::IsKeyHeld(Keyboard::LeftShift))
		{
			this->player->Sprint();
		}
		if (InputHandler::IsKeyReleased(Keyboard::LeftShift))
		{
			this->player->Walk();
		}
		if (InputHandler::IsKeyHeld(Keyboard::W))
		{
			direction.x = 1.f;
		}
		if (InputHandler::IsKeyHeld(Keyboard::S))
		{
			direction.x = -1.f;
		}
		if (InputHandler::IsKeyHeld(Keyboard::A))
		{
			direction.y = -1.f;
		}
		if (InputHandler::IsKeyHeld(Keyboard::D))
		{
			direction.y = 1.f;
		}
		if (InputHandler::IsLMBPressed())
		{
			std::cout << "X: " << this->player->GetMeshObject()->GetPosition().x << " Z: " << this->player->GetMeshObject()->GetPosition().z << "\n";



			// Test pick up
			//std::cout << "Distance to book: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(1)) << "\n";
			if (this->player->GetMeshObject()->GetDistance(SceneHandler()->EditScene().GetMeshObject(1)) < 5.0f && SceneHandler()->EditScene().GetMeshObject(1).IsVisible())
			{
				SceneHandler()->EditScene().GetMeshObject(1).SetVisible(false);
				Engine::cluesCollected++;
				Engine::playerHp += 25;
				std::cout << "Picked up Book!\n";
			}

			//std::cout << "Distance to Drawing: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(2)) << "\n";
			if (this->player->GetMeshObject()->GetDistance(SceneHandler()->EditScene().GetMeshObject(2)) < 5.0f && SceneHandler()->EditScene().GetMeshObject(2).IsVisible())
			{
				SceneHandler()->EditScene().GetMeshObject(2).SetVisible(false);
				Engine::cluesCollected++;
				Engine::playerHp += 25;
				std::cout << "Picked up Drawing!\n";
			}

			//std::cout << "Distance to Mask: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(4)) << "\n";
			if (this->player->GetMeshObject()->GetDistance(SceneHandler()->EditScene().GetMeshObject(4)) < 5.0f && SceneHandler()->EditScene().GetMeshObject(4).IsVisible())
			{
				SceneHandler()->EditScene().GetMeshObject(4).SetVisible(false);
				Engine::cluesCollected++;
				Engine::playerHp += 25;
				std::cout << "Picked up Mask!\n";
			}

			if (this->player->GetMeshObject()->GetDistance(SceneHandler()->EditScene().GetMeshObject(5)) < 5.0f && SceneHandler()->EditScene().GetMeshObject(5).IsVisible())
			{
				SceneHandler()->EditScene().GetMeshObject(5).SetVisible(false);
				Engine::cluesCollected++;
				Engine::playerHp += 25;
				std::cout << "Picked up Necklace!\n";
			}
		}
		if (InputHandler::IsRMBPressed())
		{
			if (GUIHandler::ActiveTrap())
			{
				SceneHandler()->EditScene().Add("cube.obj", "cat_bagoverhead.jpg", "", false,
					{ this->player->GetMeshObject()->GetPosition().x, 0.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
					{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }); // Rotation
			}
			else
			{
				SceneHandler()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", false,
					{ this->player->GetMeshObject()->GetPosition().x, -5.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
					{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }); // Rotation
			}
		}
		if (InputHandler::IsKeyPressed(Keyboard::E))
		{
			//this->player->Rotate(0.f, DirectX::XM_PI / 8);
			Engine::ChangeActiveTrap();
		}
		if (InputHandler::getMouseMode() == Mouse::Mode::MODE_RELATIVE && (InputHandler::GetMouseX() != 0 || InputHandler::GetMouseY() != 0))
		{
			this->player->RotateCamera(InputHandler::GetMouseY() * deltaTime, InputHandler::GetMouseX() * deltaTime);
		}
		if (InputHandler::IsKeyPressed(Keyboard::P))
		{
			GUIHandler::PauseGame();
			this->isPaused = true;
		}


		/*
			State of the art, DOUBLE C, TRIPLE B QUADRUPLE A+ system Intelligent AI!
		*/
		float speed = 0.1f;
		if (this->player->GetMeshObject()->GetPosition().x > SceneHandler()->EditScene().GetMeshObject(6).GetPosition().x)
		{
			float x = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().x;
			float z = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().z;
			SceneHandler()->EditScene().GetMeshObject(6).UpdateMatrix(
				{ x + speed, -0.0f, z },
				{ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
		}
		else if (this->player->GetMeshObject()->GetPosition().x <= SceneHandler()->EditScene().GetMeshObject(6).GetPosition().x)
		{
			float x = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().x;
			float z = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().z;
			SceneHandler()->EditScene().GetMeshObject(6).UpdateMatrix(
				{ x - speed, -0.0f, z },
				{ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
		}

		if (this->player->GetMeshObject()->GetPosition().z > SceneHandler()->EditScene().GetMeshObject(6).GetPosition().z)
		{
			float x = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().x;
			float z = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().z;
			SceneHandler()->EditScene().GetMeshObject(6).UpdateMatrix(
				{ x , -0.0f, z + speed },
				{ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
		}
		else if (this->player->GetMeshObject()->GetPosition().z <= SceneHandler()->EditScene().GetMeshObject(6).GetPosition().z)
		{
			float x = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().x;
			float z = SceneHandler()->EditScene().GetMeshObject(6).GetPosition().z;
			SceneHandler()->EditScene().GetMeshObject(6).UpdateMatrix(
				{ x , -0.0f, z - speed },
				{ 1.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 0.0f });
		}
	}
}

bool Game::OnFrame(const float& deltaTime)
{
	// Game loop
	// 1. Handle all inputs
	// 2. Update the game assets and logic
	// 3. Render

	if (Engine::cluesCollected >= CLUES)
	{
		this->menu.Switch(true);
		this->ResetValues();
		GUIHandler::ShowMainMenu(true);
		GUIHandler::ShowGameGUI(false);
	}

	HandleInput(deltaTime);

	if (this->zoomIn)
	{
		this->menu.ZoomIn({ 0.0f, 15.0f, 100.0f, 1.0f }, deltaTime, this->inGoal);
		GUIHandler::ShowMainMenu(false);
	}
	else
	{
		//Ritar ut Main Menu GUI på skärmen
		GUIHandler::ShowMainMenu(true);
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
		
		GUIHandler::ShowGameGUI(true);
		if (!enemy)
		{
			std::cout << "No monster initialized\n";
		}
		AIHandler::SetEnemy(enemy);
	}
	if (this->menu.IsInMenu() && this->hasLoaded)
	{
		// Load menu
		this->LoadMainMenu();
	}

	// Return to player buffers.
	if (this->hasLoaded)
	{
		GUIHandler::ShowMainMenu(false);
		GUIHandler::ShowGameGUI(true);
		player->Update(SceneHandler()->EditScene().GetAllMeshObjects(), direction, deltaTime);
		GUIHandler::SetPlayerPos(player->GetPlayerPos());
		AIHandler::MoveEnemy(deltaTime);
	}
	
	//Om man trycker på Resumeknappen i GUI:t ska denna bli true, annars är den false
	if (GUIHandler::ShouldResume())
		this->isPaused = false;
	//Om man trycker på Quitknappen i GUI:t ska denna bli true, annars är den false
	if (GUIHandler::ShouldQuit())
		this->running = false;
	Engine::ClearDisplay();
	Engine::Render();
	Engine::Update();




	return true;
}

bool Game::OnStart()
{
#ifdef _DEBUG
	OpenConsole();
#endif
	this->menu.Init();
	this->LoadMainMenu();

#ifdef _DEBUG
	if (!DebugInfo::Initialize())
	{
		return false;
	}
#endif

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
}

void Game::LoadMainMenu()
{
	if (this->player)
		delete this->player;

	// Refresh the game to a clean slate.
	SceneHandler()->RemoveAllScenes();
	SceneHandler()->AddScene();
	SceneHandler()->EditScene().GetParticles().SetActive(false);

	int randX = rand() % 80 - rand() % 80;
	int randZ = rand() % 60 + 10;

	SceneHandler()->EditScene().Add("ProdigiumText_TRIANGULATED.obj", "ProdigiumTextAlbedo.png", "", true, { 0.0f, 35.0f, 85.0f }

	, { -0.25f, 0.0f, 0.0f }, {1.5f, 1.5f, 1.5f});
	LightStruct L;
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { 0.0, 40.0f, 60.0f, 35.0f };
	SceneHandler()->EditScene().AddLight(L);

	// Player model
	SceneHandler()->EditScene().Add("LowPoly_Character_Menu.obj", "Char_Albedo.png", "Char_Normal.jpg", true, 
		{ (float)randX, 0.0f, (float)randZ } // Pos
	, {0.0f, 0.0f, 0.0f});

	// Terrain
	SceneHandler()->EditScene().Add("tempTerrain.obj", "dirt_color.png", "", true, { 0.0f, -6.25f, 0.0f });
	
	// Ominous House
	SceneHandler()->EditScene().Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", true, { 0.0f, 0.0f, 150.0f }, { 0.0f, 0.0f, 0.0f }, { 3.0f, 3.0f, 3.0f });
	
	// Directional light
	L.direction = { 0.f, -1.0f, -1.0f, 1.2f };
	L.attentuate = { 0.4f, 0.008f, 0.0f, 0.0f };
	L.position = { 0.0f, 20.0f, 10.0f, 25.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { -25.0f, -7.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { -25.0, 25.0f, 50.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 25.0f, -7.0f, 50.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 25.0, 25.0f, 50.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	this->hasLoaded = false;
	this->inGoal = false;
}

void Game::LoadMap()
{
	SceneHandler()->AddScene();
	this->player = new Player();
	this->enemy = new Enemy();
	Engine::SceneHandler()->EditScene().Add(this->player->GetMeshObject());

	LightStruct L;

	// Directional light
	L.direction = { 0.f, -1.0f, -1.0f, 1.2f };
	L.attentuate = { 0.4f, 0.008f, 0.0f, 0.0f };
	L.position = { 0.0f, 20.0f, 10.0f, 25.0f };
	SceneHandler()->EditScene().AddLight(L);

	DirectX::SimpleMath::Vector2 pos = this->picker.getRandomPos();
	SceneHandler()->EditScene().Add("book_OBJ.obj", "book_albedo.png", "", false, { pos.x, -3.0f, pos.y }, { 0.0f, 0.0f, 0.0f }, { 0.4f, 0.4f, 0.4f });
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
	L.position = { pos.x, 0.0f, pos.y , 5.0f };
	SceneHandler()->EditScene().AddLight(L);

	pos = this->picker.getRandomPos();
	SceneHandler()->EditScene().Add("drawing_OBJ.obj", "drawing_albedo.png", "drawing_normal.png", false, { pos.x, -3.0f, pos.y }, { 3.14159f, 3.14159f, 0.0f }, { 0.4f, 0.4f, 0.4f });
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
	L.position = { pos.x, 0.0f, pos.y, 5.0f };
	SceneHandler()->EditScene().AddLight(L);
	//Enemy ::: Currently temporary
	SceneHandler()->EditScene().Add("LowPoly_Character_Menu.obj", "Char_Albedo.png", "Char_Normal.jpg", true,
		{ (float)50, 0.0f, (float)50 } // Pos
	, { 0.0f, 0.0f, 0.0f });
	// Terrain
	SceneHandler()->EditScene().Add("tempTerrain.obj", "dirt_color.png", "", false, { 0.0f, -6.25f, 0.0f });

	pos = this->picker.getRandomPos();
	SceneHandler()->EditScene().Add("mask_OBJ.obj", "mask_albedo.png", "mask_normal.png", false, { pos.x, -3.0f, pos.y });
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
	L.position = { pos.x, 0.0f, pos.y, 5.0f };
	SceneHandler()->EditScene().AddLight(L);

	pos = this->picker.getRandomPos();
	SceneHandler()->EditScene().Add("necklace_OBJ.obj", "cat_bagoverhead.jpg", "", false, { pos.x, -3.0f, pos.y });
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
	L.position = { pos.x, 0.0f, pos.y, 5.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("cube.obj", "cat_bagoverhead.jpg", "", true,
		{ this->player->GetMeshObject()->GetPosition().x, 0.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
		{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }); // Rotation

	// Houses around the town.
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 100.0f, -7.0f, -50.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { -5.0f, -7.0f, -50.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 100.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { -5.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 225.0f, -7.0f, 125.0f }, { 0.0f, 4.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 100.0f, -7.0f, 200.0f }, { 0.0f, 0.0f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { -5.0f, -7.0f, 200.0f }, { 0.0f, 0.0f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 330.0f, -7.0f, 100.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 450.0f, -7.0f, 100.0f }, { 0.0f, 4.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 500.0f, -7.0f, -10.0f }, { 0.0f, 4.71238898f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 200.0f, -7.0f, -100.0f }, { 0.0f, 1.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 175.0f, -7.0f, -350.0f }, { 0.0f, 0.0f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 50.0f, -7.0f, -350.0f }, { 0.0f, 0.0f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 100.0f, -7.0f, -135.0f }, {0.0f, 3.14159f, 0.0f});
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { -5.0f, -7.0f, -135.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 325.0f, -7.0f, 225.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 425.0f, -7.0f, 225.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 675.0f, -7.0f, -25.0f }, { 0.0f, 4.71f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 675.0f, -7.0f, 100.0f }, { 0.0f, 4.71f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 575.0f, -7.0f, 175.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { -200.0f, -7.0f, -200.0f }, { 0.0f, 1.57f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { -200.0f, -7.0f, -330.0f }, { 0.0f, 1.57f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 125.0f, -7.0f, -240.0f }, { 0.0f, 1.57f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { 75.0f, -7.0f, -240.0f }, { 0.0f, 4.7123f, 0.0f });
	SceneHandler()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "Hus1_Normal.png", true, { -275.0f, -7.0f, 125.0f }, { 0.0f, 3.14159f, 0.0f });


	/*
		Lamps
	*/
	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 575.0f, -7.0f, 50.0f }, { 0.0f, 1.57f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 575.0f, 25.0f, 50.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { -45.0f, -7.0f, -100.0f }, { 0.0f, 1.57f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { -45.0f, 25.0f, -100.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 130.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 130.0f, 25.0f, 0.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 30.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 0.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 190.0f, -7.0f, 90.0f }, { 0.0f, 4.14159f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 190.0f, 25.0f, 90.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 30.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 100.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 130.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 130.0f, 25.0f, 100.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	SceneHandler()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { 325.0f, -7.0f, -75.0f }, { 0.0f, 1.57079633f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 325.0f, 25.0f, -75.0f, 30.0f };
	SceneHandler()->EditScene().AddLight(L);

	// Big momma house
	SceneHandler()->EditScene().Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", true, { -150.0f, 1.0f, 50.0f }, { 0.0f, 4.71238898f, 0.0f }, { 3.0f, 3.0f, 3.0f });
	SceneHandler()->EditScene().Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "Hus2_Normal.png", true, { 475.0f, 1.0f, -250.0f }, { 0.0f, 2.35619449f, 0.0f }, { 4.0f, 4.0f, 4.0f });
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { -91.0f, 12.f, 50.0f, 40.0f };
	SceneHandler()->EditScene().AddLight(L);

	// Big momma house 2
	L.direction = { 0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { 420.0f, 12.f, -195.0f, 40.0f };
	SceneHandler()->EditScene().AddLight(L);

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

		SceneHandler()->EditScene().Add("shittytree.obj", "puke_color.png", "", false, { x, -5.5f, z }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
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

		SceneHandler()->EditScene().Add("shittymountain.obj", "gray_color.png", "", true, { x, -12.5f, z }, { 0.0f, 0.0f, 0.0f }, { 10.0f, 10.0f, 10.0f });
	}

	/*
	// Random lights in the forests
	for (int j = 0; j < 5; j++)
	{
		float x = (float)(rand() % 1500 - rand() % 1500);
		float z = (float)(rand() % 1500 - rand() % 1500);
		while (x > -375 && x < 750 && z > -375 && z < 275)
		{
			x = (float)(rand() % 1500 - rand() % 1500);
			z = (float)(rand() % 1500 - rand() % 1500);
		}

		SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "Lamp1_Normal.png", true, { x, -7.0f, z }, { 0.0f, 1.57079633f, 0.0f }, { 5.0f, 5.0f, 5.0f });
		L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
		L.position = { x, 25.0f, z, 30.0f };
		SceneHandle()->EditScene().AddLight(L);
		std::cout << "Light " << j << " is at pos: " << "x:" << x << " z:" << z << "\n";
	}
	*/

	this->hasLoaded = true;
	this->inGoal = true;
}