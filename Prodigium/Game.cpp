#include "Game.h"
#include <iostream>
#include <thread>
#include "ParticleSystem.h"

Game::Game(const HINSTANCE& instance, const UINT& windowWidth, const UINT& windowHeight)
	:Engine(instance, windowWidth, windowHeight)
{
	this->running = true;
	this->isPaused = false;
	this->player = nullptr;
}

Game::~Game()
{
	if (this->player)
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
	if(!this->isPaused)
		InputHandler::UpdateKeyboardAndMouse();

	DirectX::SimpleMath::Vector2 direction(0.0f, 0.0f);

	//TODO: Make the engine cleanly shutdown
	if (InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		this->running = false;
	}
	if (InputHandler::IsKeyPressed(Keyboard::K))
	{
		//OpenConsole();
		SceneHandle()->EditScene().GetParticles().SetActive(true);
	}
	if (InputHandler::IsKeyPressed(Keyboard::L))
	{
		SceneHandle()->EditScene().GetParticles().SetActive(false);
	}
	if(InputHandler::IsKeyHeld(Keyboard::LeftShift))
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
		//std::cout << "X: " << InputHandler::GetMouseX() << " Y: " << InputHandler::GetMouseY() << "\n";

		// Test pick up
		std::cout << "Distance to book: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(1)) << "\n";
		if (this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(1)) < 5.0f)
		{
			SceneHandle()->EditScene().GetMeshObject(1).SetVisible(false);
			std::cout << "Picked up Book!\n";
		}

		std::cout << "Distance to Drawing: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(2)) << "\n";
		if (this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(2)) < 5.0f)
		{
			SceneHandle()->EditScene().GetMeshObject(2).SetVisible(false);
			std::cout << "Picked up Drawing!\n";
		}

		std::cout << "Distance to Mask: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(4)) << "\n";
		if (this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(4)) < 5.0f)
		{
			SceneHandle()->EditScene().GetMeshObject(4).SetVisible(false);
			std::cout << "Picked up Mask!\n";
		}
	}
	if (InputHandler::IsRMBPressed())
	{
		// Temp -- Change to trap later
		SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", true,
			{ this->player->GetMeshObject()->GetPosition().x, -5.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
			{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }); // Rotation
	}
	if (InputHandler::IsKeyPressed(Keyboard::E))
	{
		//this->player->Rotate(0.f, DirectX::XM_PI / 8);
		Engine::ChangeActiveTrap();
	}
	if (InputHandler::IsKeyPressed(Keyboard::P))
	{
		Engine::PauseGame();
		this->isPaused = true;
	}
	if (InputHandler::IsKeyPressed(Keyboard::T))
	{
		//this->player->Rotate(DirectX::XM_PI / 8, 0.f);
	}
	if (InputHandler::getMouseMode() == Mouse::Mode::MODE_RELATIVE && (InputHandler::GetMouseX() != 0 || InputHandler::GetMouseY() != 0))
	{
		this->player->Rotate(InputHandler::GetMouseY() * deltaTime, InputHandler::GetMouseX() * deltaTime);
	}

	if (direction.Length() > 0.0f)
	{
		this->player->Move(direction, deltaTime);
	}
}

bool Game::OnFrame(const float& deltaTime)
{
	// Game loop
	// 1. Handle all inputs
	// 2. Update the game assets and logic
	// 3. Render

	HandleInput(deltaTime);

	player->Update(deltaTime);
	Engine::SetPlayerPos(player->GetPlayerPos());
	for (int i = 1; i < SceneHandle()->EditScene().GetNumberOfObjects(); i++)
	{
		if (player->CheckCollision(&SceneHandle()->EditScene().GetMeshObject(i)))
		{
			std::cout << i << std::endl;
		}
	}

	Engine::ClearDisplay();
	Engine::Render();

	return true;
}

bool Game::OnStart()
{
	OpenConsole();
	this->LoadMap();

#ifdef _DEBUG
	if (!DebugInfo::Initialize())
	{
		return false;
	}
#endif

	return true;
}

void Game::LoadMap()
{
	SceneHandle()->AddScene();
	this->player = new Player();
	Engine::SceneHandle()->EditScene().Add(this->player->GetMeshObject());

	LightStruct L;

	// Directional light
	L.direction = { 0.f, -1.0f, -1.0f, 1.2f };
	L.attentuate = { 0.4f, 0.008f, 0.0f, 0.0f };
	L.position = { 0.0f, 20.0f, 10.0f, 25.0f };
	SceneHandle()->EditScene().AddLight(L);

	// Test pickups but its static lol
	float randX = (float)(rand() % 200 - rand() % 200);
	float randZ = (float)(rand() % 200- rand() % 200);
	SceneHandle()->EditScene().Add("book_OBJ.obj", "book_albedo.png", "", true, { randX, -3.0f, randZ }, { 0.0f, 0.0f, 0.0f }, { 0.4f, 0.4f, 0.4f });

	randX = (float)(rand() % 200 - rand() % 200);
	randZ = (float)(rand() % 200 - rand() % 200);
	SceneHandle()->EditScene().Add("drawing_OBJ.obj", "drawing_albedo.png", "", true, { randX, -3.0f, randZ }, { 3.14159f, 3.14159f, 0.0f }, { 0.4f, 0.4f, 0.4f });

	// Terrain
	SceneHandle()->EditScene().Add("tempTerrain.obj", "dirt_color.png", "", false, { 0.0f, -6.25f, 0.0f });

	randX = (float)(rand() % 200 - rand() % 200);
	randZ = (float)(rand() % 200 - rand() % 200);
	SceneHandle()->EditScene().Add("mask_OBJ.obj", "mask_albedo.png", "", true, { randX, -3.0f, randZ });

	// Houses around the town.
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { 100.0f, -7.0f, -50.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { -5.0f, -7.0f, -50.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { 100.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { -5.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { 225.0f, -7.0f, 125.0f }, { 0.0f, 4.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { 330.0f, -7.0f, 100.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { 450.0f, -7.0f, 100.0f }, { 0.0f, 4.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { 500.0f, -7.0f, -10.0f }, { 0.0f, 4.71238898f, 0.0f });
	SceneHandle()->EditScene().Add("House1_SubMeshes.obj", "Hus1_Diffuse.png", "", true, { 200.0f, -7.0f, -100.0f }, { 0.0f, 1.14159f, 0.0f });

	/*
		Lamps
	*/
	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "", true, { 130.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 130.0f, 25.0f, 0.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "", true, { 30.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 0.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "", true, { 190.0f, -7.0f, 90.0f }, { 0.0f, 4.14159f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 190.0f, 25.0f, 90.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "", true, { 30.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 100.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "", true, { 130.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 130.0f, 25.0f, 100.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1_SubMesh.obj", "Lamp1_Diffuse.png", "", true, { 325.0f, -7.0f, -75.0f }, { 0.0f, 1.57079633f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 325.0f, 25.0f, -75.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	// Big momma house
	SceneHandle()->EditScene().Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "", true, { -150.0f, 1.0f, 50.0f }, { 0.0f, 4.71238898f, 0.0f }, { 3.0f, 3.0f, 3.0f });
	SceneHandle()->EditScene().Add("House2_SubMeshes.obj", "Hus2_Diffuse.png", "", true, { 475.0f, 1.0f, -250.0f }, { 0.0f, 2.35619449f, 0.0f }, { 4.0f, 4.0f, 4.0f });
	L.direction = { -0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
	L.position = { -91.0f, 12.f, 50.0f, 40.0f };
	SceneHandle()->EditScene().AddLight(L);

	// Big momma house 2
	L.direction = { 0.3f, 1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.5f, 0.0f, 1.0f };
	L.position = { 420.0f, 12.f, -195.0f, 40.0f };
	SceneHandle()->EditScene().AddLight(L);

	// Tree galore!! aka Performance test
	for (int i = 0; i < 500; i++)
	{
		float x = (float)(rand() % 1000 - rand() % 1000);
		float z = (float)(rand() % 1000 - rand() % 1000);
		while (x > -300 && x < 700 && z > -300 && z < 150)
		{
			x = (float)(rand() % 1000 - rand() % 1000);
			z = (float)(rand() % 1000 - rand() % 1000);
		}
		
		SceneHandle()->EditScene().Add("shittytree.obj", "puke_color.png", "", false, { x, -5.5f, z }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	}
}