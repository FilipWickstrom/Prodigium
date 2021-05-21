#pragma once

#define MAINMENU 1
#define INGAME 2

class OptionsHandler
{
public:
	/*
		Contains all the values that are saved and used for options menu.
	*/

	// Difficulty affects the time it takes to replace traps.
	int difficulty;

	// Volume settings.
	float masterVolume;
	float sfxVolume;
	float ambientVolume;
	float musicVolume;

	// Timer
	float gameTimer;

	// Keeps track of where the player is. Menu? Game? Options?
	int state;

	// Increase performance by disabling options
	bool hasParticles;
	bool hasBlur;
	bool hasDebugInfo;

	// Control affecting attributes
	float mouseSens;
	bool inverseSens;

	OptionsHandler();
	~OptionsHandler();
};