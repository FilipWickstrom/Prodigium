#include "Game.h"
#include <iostream>


Game::Game(HINSTANCE& instance, UINT width, UINT height)
	:Engine(instance, width, height)
{
	this->running = true;
	this->player = nullptr;
	
}

Game::~Game()
{
	if (this->player)
		delete this->player;
}

bool Game::IsRunning() const
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

	DirectX::SimpleMath::Vector3 direction(0.f, 0.f, 0.f);

	//TODO: Make the engine cleanly shutdown
	if (InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		this->running = false;
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
		direction.z += 1.f;
	}
	if (InputHandler::IsKeyHeld(Keyboard::S))
	{
		direction.z += -1.f;
	}
	if (InputHandler::IsKeyHeld(Keyboard::A))
	{
		direction.x += -1.f;
	}
	if (InputHandler::IsKeyHeld(Keyboard::D))
	{
		direction.x += 1.f;
	}
	if (InputHandler::IsLMBPressed())
	{
		std::cout << "X: " << InputHandler::GetMouseX() << " Y: " << InputHandler::GetMouseY() << "\n";
	}
	if (InputHandler::IsKeyPressed(Keyboard::E))
	{
		this->player->Rotate(0.f, DirectX::XM_PI / 8);
	}
	if (InputHandler::IsKeyPressed(Keyboard::T))
	{
		this->player->Rotate( DirectX::XM_PI / 8, 0.f);
	}
	if (InputHandler::getMouseMode() == Mouse::Mode::MODE_RELATIVE)
	{
		this->player->Rotate(InputHandler::GetMouseY() * deltaTime, InputHandler::GetMouseX() * deltaTime);
	}

	this->player->Move(direction, deltaTime);
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

bool Game::LoadMap()
{
	SceneHandle()->AddScene();
	this->player = new Player();
	Engine::SceneHandle()->EditScene().Add(this->player->GetMeshObject());

	LightStruct L;
	L.direction = { 1.f, -1.0f, 0.0f, 1.2f };
	L.attentuate = { 0.4f, 0.008f, 0.0f, 0.0f };
	L.position = { 0.0f, 20.0f, 10.0f, 25.0f };
	SceneHandle()->EditScene().AddLight(L);


	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", {0.0f, -7.0f, -50.0f});
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 100.0f, -7.0f, -50.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 100.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 0.0f, -7.0f, 150.0f }, { 0.0f, 3.14159f, 0.0f });
	SceneHandle()->EditScene().Add("House1.obj", "Hus1_Diffuse.png", "", { 225.0f, -7.0f, 125.0f }, { 0.0f, 4.14159f, 0.0f });
	SceneHandle()->EditScene().Add("tempTerrain.obj", "dirt_color.png", "", { 0.0f, -6.25f, 0.0f });

	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 100.0f, -7.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.direction = { 0.f, -1.0f, 0.0f, 1.5f };
	L.attentuate = { 0.4f, 0.008f, 0.003f, 2.0f };
	L.position = { 100.0f, 20.0f, 0.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);
	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 50.0f, -6.5f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 50.0f, 20.0f, 0.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);
	SceneHandle()->EditScene().Add("Lamp1.obj", "Lamp1_Diffuse.png", "", { 190.0f, -7.0f, 90.0f }, { 0.0f, 0.0f, 0.0f }, { 5.0f, 5.0f, 5.0f });
	L.position = { 190.0f, 20.0f, 90.0f, 30.0f };
	SceneHandle()->EditScene().AddLight(L);

	SceneHandle()->EditScene().Add("House2.obj", "Hus2_Diffuse.png", "", { -150.0f, -6.5f, 50.0f }, { 0.0f, 4.71238898f, 0.0f }, {3.0f, 3.0f, 3.0f});
	SceneHandle()->EditScene().Add("book_OBJ.obj", "book_albedo.png", "", { 42.0f, -3.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, {0.4f, 0.4f, 0.4f});
	SceneHandle()->EditScene().Add("drawing_OBJ.obj", "drawing_albedo.png", "", { 37.0f, -3.0f, 0.0f }, { 3.14159f, 3.14159f, 0.0f }, { 0.4f, 0.4f, 0.4f });

	return false;
}
