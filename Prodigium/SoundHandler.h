#pragma once
#include <Audio.h>
#include <SimpleMath.h>
#include <iostream>


class SoundHandler
{
private:
	std::unique_ptr<DirectX::AudioEngine> audEngine;
	std::unique_ptr<DirectX::WaveBank> waveBankFX;
	std::unique_ptr<DirectX::WaveBank> waveBankAmbient;
	std::unique_ptr<DirectX::WaveBank> waveBankMusic;
	std::unique_ptr<DirectX::SoundEffectInstance> instanceFX;
	std::unique_ptr<DirectX::SoundEffectInstance> instanceMusic;
	std::unique_ptr<DirectX::SoundEffectInstance> instanceAmbient;
	float masterVolume;
	float ambientVolume;
	float musicVolume;
	float fxVolume;
public:
	SoundHandler();
	virtual ~SoundHandler();
	const bool Initialize();
	void Update();
	void SetMasterVolume(const float& newVolume);
	void SetAmbientVolume(const float& newVolume);
	void SetMusicVolume(const float& newVolume);
	void SetFXVolume(const float& newVolume);
	void PlayOneShot(const int& index); 
	void PlayLooping(const int& index, const bool& use3D = false, const DirectX::SimpleMath::Vector3& listnerPos = { 0,0,0 }, 
					 const DirectX::SimpleMath::Vector3& emitterPos = { 0,0,0 });
	void PlayMusic(const int& index);
	void PlayAmbient(const int& index);
	void SuspendAudio();
	void ResumeAudio();
};