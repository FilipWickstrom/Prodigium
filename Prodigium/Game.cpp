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
		this->player->Rotate(DirectX::XM_PI / 8, 0.f);
	}
	if (InputHandler::getMouseMode() == Mouse::Mode::MODE_RELATIVE && (InputHandler::GetMouseX() != 0 || InputHandler::GetMouseY() != 0))
	{
		this->player->Rotate(InputHandler::GetMouseY() * deltaTime, InputHandler::GetMouseX() * deltaTime);
	}

	if (direction.Length() > 0.f)
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
	this->player = new Player();
	Engine::SceneHandle()->EditScene().Add(this->player->GetMeshObject());

	return true;
}