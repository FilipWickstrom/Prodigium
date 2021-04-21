#include "Game.h"
#include <iostream>


Game::Game(HINSTANCE& instance, UINT width, UINT height)
	:Engine(instance, width, height)
{
}

Game::~Game()
{
}

bool Game::OnFrame(const float& deltaTime)
{
	Engine::ClearDisplay();
	Engine::Render();

	return true;
}

bool Game::OnStart()
{


	return true;
}
