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
	return this->running;
}

bool Game::OnFrame(const float& deltaTime)
{
	Engine::ClearDisplay();
	auto kb = InputHandler::GetKBState();
	auto kb_keys = InputHandler::GetKBStateTracker();
	kb_keys->Update(kb);
	auto mouse = InputHandler::GetMouseState();
	if (kb_keys->pressed.Escape)
	{
		running = false;
	}
	if (kb_keys->pressed.Tab)
	{
		std::cout << "pls fucking work\n";
	}
	if (kb_keys->IsKeyPressed(Keyboard::W))
	{
		std::cout << "Hello!\n";
	}
	if (kb_keys->IsKeyReleased(Keyboard::W))
	{
		std::cout << "Goodbye!\n";
	}
	if (mouse.leftButton)
	{
		std::cout << "X: " << mouse.x << " Y: " << mouse.y << "\n";
	}
	Engine::Render();

	return true;
}

bool Game::OnStart()
{


	return true;
}
