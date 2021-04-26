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

void Game::HandleInput(float deltaTime)
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
		//std::cout << Engine::gameCam.getPos().x << " " << Engine::gameCam.getPos().y << " "<< Engine::gameCam.getPos().z << std::endl;
		Engine::gameCam.Move(0, 0.4f * deltaTime);

	}	
	if (InputHandler::IsKeyHeld(Keyboard::S))
	{
		//std::cout << "Holding key!\n";
		Engine::gameCam.Move(0,-0.4f * deltaTime);
	}
	if (InputHandler::IsLMBPressed())
	{
		std::cout << "X: " << InputHandler::GetMouseX() << " Y: " << InputHandler::GetMouseY() << "\n";
	}
}

bool Game::OnFrame(const float& deltaTime)
{
	HandleInput();
	Engine::ClearDisplay();
	HandleInput(deltaTime);
	Engine::Render();

	return true;
}

bool Game::OnStart()
{

	//if (!this->tempCam.Initialize(1280, 1024, 0.1f, 10000.0f, XM_PI * 0.5f, { 0.f, 0.f, -5.f }))
	//{
	//	std::cout << "CAMERA ERROR\n";
	//	return false;
	//}

	return true;
}
