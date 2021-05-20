#include "OptionsHandler.h"
#include <fstream>
#include <string>
#include <sstream>
OptionsHandler::OptionsHandler()
{
	// Default settings
	this->masterVolume = 1.0f;
	this->sfxVolume = 0.3f;
	this->ambientVolume = 0.3f;
	this->musicVolume = 0.2f;
	this->difficulty = 1;
	this->gameTimer = 0;
	this->state = MAINMENU;

	this->hasBlur = true;
	this->hasParticles = true;

	std::fstream input;
	input.open("Settings/audiosettings.txt", std::fstream::in);
	std::string line;
	while (std::getline(input, line))
	{
		std::stringstream ss(line);
		std::string type;
		float value;
		ss >> type >> value;

		if (type == "master")
			this->masterVolume = value;

		else if (type == "sfx")
			this->sfxVolume = value;

		else if (type == "music")
			this->musicVolume = value;

		else if (type == "ambient")
			this->ambientVolume = value;

		// Secret own defined difficulty in .txt file.
		else if (type == "difficulty")
			this->difficulty = value;
	}
	input.close();

	if (this->masterVolume > 1.0f)
		this->masterVolume = 1.0f;
	if (this->ambientVolume > 1.0f)
		this->ambientVolume = 1.0f;
	if (this->musicVolume > 1.0f)
		this->musicVolume = 1.0f;
	if (this->sfxVolume > 1.0f)
		this->sfxVolume = 1.0f;
}

OptionsHandler::~OptionsHandler()
{
	// Save settings onto a .txt file
	std::fstream output;
	output.open("Settings/audiosettings.txt", std::fstream::out);
	output << "master " << this->masterVolume;
	output << "\nsfx " << this->sfxVolume;
	output << "\nmusic " << this->musicVolume;
	output << "\nambient " << this->ambientVolume;
	output.close();
}
