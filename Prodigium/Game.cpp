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

	//TODO: Make the engine cleanly shutdown
	if (InputHandler::IsKeyPressed(Keyboard::Escape))
	{
		this->running = false;
	}

	if (InputHandler::IsKeyHeld(Keyboard::W))
	{
		this->player->Move(0, 1.f, deltaTime);
	}
	if (InputHandler::IsKeyHeld(Keyboard::S))
	{
		this->player->Move(0, -1.f, deltaTime);
	}
	if (InputHandler::IsKeyHeld(Keyboard::A))
	{
		this->player->Move(-1.f, 0, deltaTime);
	}
	if (InputHandler::IsKeyHeld(Keyboard::D))
	{
		this->player->Move(1.f, 0, deltaTime);
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