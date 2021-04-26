#include "Game.h"
#include <iostream>


Game::Game(HINSTANCE& instance, UINT width, UINT height)
	:Engine(instance, width, height)
{
	this->running = true;
}

Game::~Game()
{
}

bool Game::IsRunning() const
{
	if (this->running == false || InputHandler::IsRunning() == false)
	{
		return false;
	}
	
	return true;
}

void Game::HandleInput()
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
		std::cout << "Hello!\n";
	}
	if (InputHandler::IsKeyHeld(Keyboard::E))
	{
		std::cout << "Holding key!\n";
	}
	if (InputHandler::IsLMBPressed())
	{
		std::cout << "X: " << InputHandler::GetMouseX()<< " Y: " << InputHandler::GetMouseY() << "\n";
	}
}

bool Game::OnFrame(const float& deltaTime)
{
	Engine::ClearDisplay();
	HandleInput();
	Engine::Render();
	HandleInput();

	return true;
}

bool Game::OnStart()
{


	return true;
}
