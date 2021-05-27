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
	std::unique_ptr<DirectX::WaveBank> waveBankMonster;
	std::unique_ptr<DirectX::SoundEffectInstance> instanceFX;
	std::unique_ptr<DirectX::SoundEffectInstance> instanceMusic;
	std::unique_ptr<DirectX::SoundEffectInstance> instanceAmbient;
	std::unique_ptr<DirectX::SoundEffectInstance> instanceMonster;
	DirectX::AudioListener listner;
	DirectX::AudioEmitter emitter;
	float masterVolume;
	float ambientVolume;
	float musicVolume;
	float fxVolume;
public:
	SoundHandler();
	virtual ~SoundHandler();
	const bool Initialize();
	void Update(const DirectX::SimpleMath::Vector3& listnerPos, const DirectX::SimpleMath::Vector3& emitterPos, 
				const DirectX::SimpleMath::Vector3& listnerForward, const DirectX::SimpleMath::Vector3& emitterForward);
	void SetMasterVolume(const float& newVolume);
	void SetAmbientVolume(const float& newVolume);
	void SetMusicVolume(const float& newVolume);
	void SetFXVolume(const float& newVolume);
	void PlayOneShot(const int& index);
	void PlayMonsterSounds(const int& index);
	void PlayMusic(const int& index);
	void PlayAmbient(const int& index);
	void SetPitch(const float& newPitch);
	void SuspendAudio();
	void ResumeAudio();
};