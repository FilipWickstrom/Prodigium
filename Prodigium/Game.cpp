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

	if (InputHandler::IsKeyPressed(Keyboard::W))
	{
		//std::cout << "Hello!\n";
	}
	if (InputHandler::IsKeyHeld(Keyboard::W))
	{
		//std::cout << "Holding key!\n";
		this->player->Move(0, 0.4f * deltaTime);
	}	
	if (InputHandler::IsKeyHeld(Keyboard::S))
	{
		this->player->Move(0, -0.4f * deltaTime);
	}
	if (InputHandler::IsLMBPressed())
	{
		std::cout << "X: " << InputHandler::GetMouseX() << " Y: " << InputHandler::GetMouseY() << "\n";
	}
}

bool Game::OnFrame(const float& deltaTime)
{
	HandleInput(deltaTime);
	player->Update(deltaTime);
	Engine::ClearDisplay();
	Engine::Render();

	return true;
}

bool Game::OnStart()
{
	this->player = new Player();

	return true;
}
