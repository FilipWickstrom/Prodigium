#include "Game.h"
#include <iostream>
#include <thread>
#include "ParticleSystem.h"

Game::Game(const HINSTANCE& instance, const UINT& windowWidth, const UINT& windowHeight)
	:Engine(instance, windowWidth, windowHeight)
{
	this->running = true;
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
	InputHandler::UpdateKeyboardAndMouse();

	DirectX::SimpleMath::Vector2 direction(0.0f, 0.0f);

	//TODO: Make the engine cleanly shutdown
	if (InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		this->running = false;
	}
	if (InputHandler::IsKeyPressed(Keyboard::K))
	{
		OpenConsole();
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
		std::cout << "X: " << InputHandler::GetMouseX() << " Y: " << InputHandler::GetMouseY() << "\n";

		// Test pick up
		std::cout << "Distance to book: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(1)) << "\n";
		if (this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(1)) < 5.0f)
		{
			SceneHandle()->EditScene().RemoveObject(1);
			std::cout << "Picked up Book!\n";
		}

		std::cout << "Distance to Drawing: " << this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(2)) << "\n";
		if (this->player->GetMeshObject()->GetDistance(SceneHandle()->EditScene().GetMeshObject(2)) < 5.0f)
		{
			SceneHandle()->EditScene().RemoveObject(2);
			std::cout << "Picked up Drawing!\n";
		}
	}
	if (InputHandler::IsRMBPressed())
	{
		// Temp -- Change to trap later
		SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "",
			{ this->player->GetMeshObject()->GetPosition().x, -5.0f, this->player->GetMeshObject()->GetPosition().z }, // Position
			{ this->player->GetMeshObject()->GetRotation().x, this->player->GetMeshObject()->GetRotation().y, this->player->GetMeshObject()->GetRotation().z }); // Rotation
	}
	if (InputHandler::IsKeyPressed(Keyboard::E))
	{
		//this->player->Rotate(0.f, DirectX::XM_PI / 8);
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

	Engine::ClearDisplay();
	Engine::Render();

	return true;
}

bool Game::OnStart()
{
	this->LoadMap();
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
	SceneHandle()->EditScene().Add("book_OBJ.obj", "book_albedo.png", "", { randX, -3.0f, randZ }, { 0.0f, 0.0f, 0.0f }, { 0.4f, 0.4f, 0.4f });

	randX = (float)(rand() % 200 - rand() % 200);
	randZ = (float)(rand() % 200 - rand() % 200);
	SceneHandle()->EditScene().Add("drawing_OBJ.obj", "drawing_albedo.png", "", { randX, -3.0f, randZ }, { 3.14159f, 3.14159f, 0.0f }, { 0.4f, 0.4f, 0.4f });

	// Terrain
	SceneHandle()->EditScene().Add("tempTerrain.obj", "dirt_color.png", "", { 0.0f, -6.25f, 0.0f });

	// Houses around the town.
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", {-5.0f, -7.0f, -50.0f});
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 100.0f, -7.0f, -50.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 100.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { -5.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 225.0f, -7.0f, 125.0f }, { 0.0f, 4.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 330.0f, -7.0f, 100.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 450.0f, -7.0f, 100.0f }, { 0.0f, 4.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 500.0f, -7.0f, -10.0f }, { 0.0f, 4.71238898f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 200.0f, -7.0f, -100.0f }, { 0.0f, 1.14159f, 0.0f });

	/*
		Lamps
	*/
	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 130.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 130.0f, 20.0f, 0.0f, 20.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 30.0f, -6.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 0.0f, 20.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 190.0f, -7.0f, 90.0f }, { 0.0f, 4.14159f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 190.0f, 25.0f, 90.0f, 20.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 30.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 30.0f, 25.0f, 100.0f, 20.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 130.0f, -7.0f, 100.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 130.0f, 25.0f, 100.0f, 20.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 325.0f, -7.0f, -75.0f }, { 0.0f, 1.57079633f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.attentuate = { 0.032f, 0.003f, 0.0f, 2.0f };
	L.position = { 325.0f, 25.0f, -75.0f, 20.0f };
	SceneHandle()->EditScene().AddLight(L);

	// Big momma house
	SceneHandle()->EditScene().Add("House2.obj", "Hus2_Diffuse.png", "", { -150.0f, -6.5f, 50.0f }, { 0.0f, 4.71238898f, 0.0f }, {3.0f, 3.0f, 3.0f});
	SceneHandle()->EditScene().Add("House2.obj", "Hus2_Diffuse.png", "", { 475.0f, -7.0f, -250.0f }, { 0.0f, 2.35619449f, 0.0f }, { 4.0f, 4.0f, 4.0f });
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { -75.0f, 20.0f, 45.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	// Big momma house 2
	L.attentuate = { 0.4f, 0.5f, 0.0f, 2.0f };
	L.position = { 375.0f, 25.0f, -175.0f, 30.0f };
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
		
		SceneHandle()->EditScene().Add("shittytree.obj", "puke_color.png", "", { x, -5.5f, z }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	}

	//Now works loading in object-files with more than one meshes
	SceneHandle()->EditScene().Add("Lamp2.obj", "Lampa2_Diffuse.png", "", {0,0,0}, {0,0,0}, {2.0f, 2.0f, 2.0f});
}
