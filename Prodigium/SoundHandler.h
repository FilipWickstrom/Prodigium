#pragma once
#include <Audio.h>
#include <SimpleMath.h>
#include <iostream>
#include <string>
#include <memory>
#include "UsefulHeader.h"


class SoundHandler
{
private:
	std::unique_ptr<DirectX::AudioEngine> audEngine;
	std::unique_ptr<DirectX::SoundEffect> soundEffect;
public:
	//DELETE_COPY_ASSIGNMENT(SoundHandler);
	SoundHandler();
	virtual ~SoundHandler();
	const bool Initialize();
	void Update();
	//void PlayOneShot(const std::string& fileName);
	void PlayOneShot();
	//void PlayLooping(const std::string& fileName, const bool& play3D, const DirectX::SimpleMath::Vector3& listnerPos, const DirectX::SimpleMath::Vector3& emitterPos);
	//void PlayLooping(const std::string& fileName);
};