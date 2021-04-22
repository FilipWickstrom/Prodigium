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
	if (kb.Escape)
	{
		running = false;
	}
	Engine::Render();

	return true;
}

bool Game::OnStart()
{


	return true;
}
