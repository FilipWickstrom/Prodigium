#pragma once
#include <Audio.h>
#include <SimpleMath.h>
#include <iostream>
#include <string>
#include <wchar.h>
#include <memory>
#include "UsefulHeader.h"


class SoundHandler
{
private:
	DirectX::AudioEngine* audEngine;
	DirectX::SoundEffect* soundEffect;
public:
	//DELETE_COPY_ASSIGNMENT(SoundHandler);
	SoundHandler();
	virtual ~SoundHandler();
	const bool Initialize();
	void Update();
	void PlayOneShot(const wchar_t* fileName); 
	void PlayLooping(const wchar_t* fileName, const bool& play3D, const DirectX::SimpleMath::Vector3& listnerPos, const DirectX::SimpleMath::Vector3& emitterPos);
	void PlayLooping(const wchar_t* fileName);
};