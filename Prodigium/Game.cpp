#include "Game.h"
#include <iostream>

Game::Game()
{

}

Game::~Game()
{
}

bool Game::Run(HINSTANCE& instance, UINT width, UINT height)
{
	if (!engine.StartUp(instance, width, height))
	{
		return false;
	}

	return true;
}

bool Game::OnFrame(const float& deltaTime)
{
	engine.ClearDisplay();
	engine.PresentScene();
	return true;
}
