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
	std::unique_ptr<DirectX::AudioEngine> audEngine;
	std::unique_ptr<DirectX::SoundEffect> soundEffect;
	std::unique_ptr<DirectX::WaveBank> waveBank;
	std::unique_ptr<DirectX::SoundEffectInstance> instance;
	float masterVolume;
public:
	//DELETE_COPY_ASSIGNMENT(SoundHandler);
	SoundHandler();
	virtual ~SoundHandler();
	const bool Initialize();
	void Update();
	void SetVolume(const float& newVolume);
	void PlayOneShot(const int& index); 
	void PlayLooping(const int& index, const bool& use3D = false, const DirectX::SimpleMath::Vector3& listnerPos = { 0,0,0 }, 
					 const DirectX::SimpleMath::Vector3& emitterPos = { 0,0,0 });
	void SuspendAudio();
	void ResumeAudio();
};