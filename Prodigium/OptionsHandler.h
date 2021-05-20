#pragma once
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

	OptionsHandler();
	~OptionsHandler();
};