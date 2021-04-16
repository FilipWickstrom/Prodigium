#include "Game.h"
#include <iostream>

Game::Game()
{
}

Game::~Game()
{
}

bool Game::Start(HINSTANCE& instance, UINT width, UINT height)
{
	if (!engine.StartUp(instance, width, height))
	{
		std::cout << "Couldn't initialize engine, aborting!" << std::endl;
		return false;
	}

	return true;
}

bool Game::OnFrame(const float& deltaTime)
{
	engine.PresentScene();
	engine.ClearDisplay();

	return true;
}
