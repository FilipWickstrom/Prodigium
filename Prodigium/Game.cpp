#include "Game.h"
#include <iostream>

Game::Game()
{
}

Game::~Game()
{
}

void Game::Run(HINSTANCE& instance, UINT width, UINT height)
{
	if (!engine.StartUp(instance, width, height))
	{
		std::cout << "Couldn't initialize engine, aborting!" << std::endl;
	}

}

bool Game::OnFrame(const float& deltaTime)
{
	engine.ClearDisplay();
	engine.PresentScene();

	return true;
}
