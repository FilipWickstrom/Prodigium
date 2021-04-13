#pragma once
#include "Engine.h"

/*
	#include should be inside .cpp file for better start up perfomance?
*/

class Game
{
private:
	Engine engine;
	/*
		General Window and Game start up values.
	*/


	/*
		Setups
	*/

public:
	Game();
	~Game();

	// Main loop of the game.
	void Run(HINSTANCE& instance, UINT width, UINT height);

	// Start up for device. Run only once outside of loop.
	bool OnFrame(const float& deltaTime);
};