#pragma once
#include <xaudio2.h>
#include <x3daudio.h>


class SoundHandler
{
private:
public:
	SoundHandler();
	virtual ~SoundHandler();
	bool Initialize();
};