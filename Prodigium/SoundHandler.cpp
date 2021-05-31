#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	this->audEngine = nullptr;
	this->waveBankFX = nullptr;
	this->waveBankAmbient = nullptr;
	this->waveBankMusic = nullptr;
	this->waveBankMonster = nullptr;
	this->instanceFX = nullptr;
	this->instanceAmbient = nullptr;
	this->instanceMusic = nullptr;
	this->instanceMonster = nullptr;
	this->masterVolume = 1.f;
	this->ambientVolume = 1.f;
	this->musicVolume = 1.f;
	this->fxVolume = 1.f;
}

SoundHandler::~SoundHandler()
{
}

const bool SoundHandler::Initialize()
{
	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return false;

	DirectX::AUDIO_ENGINE_FLAGS eFlags = DirectX::AudioEngine_Default | DirectX::AudioEngine_EnvironmentalReverb 
									   | DirectX::AudioEngine_ReverbUseFilters;
#ifdef _DEBUG
	eFlags |= DirectX::AudioEngine_Debug;
#endif

	this->audEngine = std::make_unique<DirectX::AudioEngine>( eFlags );
	this->audEngine->SetMasterVolume(this->masterVolume);
	this->audEngine->SetReverb(DirectX::Reverb_City);
	this->waveBankFX = std::make_unique<DirectX::WaveBank>(this->audEngine.get(), L"media/sfx.xwb");
	this->waveBankAmbient = std::make_unique<DirectX::WaveBank>(this->audEngine.get(), L"media/ambient.xwb");
	this->waveBankMusic = std::make_unique<DirectX::WaveBank>(this->audEngine.get(), L"media/music.xwb");
	this->waveBankMonster = std::make_unique<DirectX::WaveBank>(this->audEngine.get(), L"media/monster.xwb");

	int index = 0;
	this->instanceFX = this->waveBankFX->CreateInstance(index);
	this->instanceAmbient = this->waveBankAmbient->CreateInstance(index);
	this->instanceMusic = this->waveBankMusic->CreateInstance(index);
	this->instanceMonster = this->waveBankMonster->CreateInstance(index);

	this->emitter.ChannelCount = 2;
	this->emitter.CurveDistanceScaler = 75.f;

	return true;
}

void SoundHandler::Update(const DirectX::SimpleMath::Vector3& listnerPos, const DirectX::SimpleMath::Vector3& emitterPos,
						  const DirectX::SimpleMath::Vector3& listnerForward, const DirectX::SimpleMath::Vector3& emitterForward)
{
	if (!this->audEngine->Update())
	{
		std::cout << "No audio playing!" << std::endl;

		if (this->audEngine->IsCriticalError())
		{
			std::cerr << "Critical Error Occurred with Audio Device!" << std::endl;
		}
	}
	this->listner.SetOrientation(listnerForward, DirectX::SimpleMath::Vector3(0, 1, 0));
	this->listner.Update(listnerPos, DirectX::SimpleMath::Vector3(0, 1, 0), 1);
	this->emitter.SetOrientation(emitterForward, DirectX::SimpleMath::Vector3(0, 1, 0));
	this->emitter.Update(emitterPos, DirectX::SimpleMath::Vector3(0, 1, 0), 1);
	this->instanceMonster->Apply3D(this->listner, this->emitter);
	this->audEngine->Update();
}

void SoundHandler::SetMasterVolume(const float& newVolume)
{
	this->masterVolume = newVolume;
	this->audEngine->SetMasterVolume(this->masterVolume);
}

void SoundHandler::SetAmbientVolume(const float& newVolume)
{
	this->ambientVolume = newVolume;
	this->instanceAmbient->SetVolume(newVolume);
}

void SoundHandler::SetMusicVolume(const float& newVolume)
{
	this->musicVolume = newVolume;
	this->instanceMusic->SetVolume(newVolume);
}

void SoundHandler::SetFXVolume(const float& newVolume)
{
	this->fxVolume = newVolume;
	this->instanceFX->SetVolume(newVolume);
	this->instanceMonster->SetVolume(newVolume * 10.f);
}

void SoundHandler::PlayOneShot(const int& index)
{
	this->instanceFX = this->waveBankFX->CreateInstance(index);
	if (!this->instanceFX)
		std::cout << "Index not in wave bank!" << std::endl;
	else
		this->instanceFX->Play();
}

void SoundHandler::PlayMonsterSounds(const int& index)
{
	DirectX::SOUND_EFFECT_INSTANCE_FLAGS flags = DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters;
	this->instanceMonster = this->waveBankMonster->CreateInstance(index, flags);
	if(!this->instanceMonster)
		std::cout << "Index not in wave bank!" << std::endl;
	else
	{
		this->instanceMonster->Play();
	}
}

void SoundHandler::PlayMusic(const int& index)
{
	this->instanceMusic = this->waveBankMusic->CreateInstance(index);
	if(!this->instanceMusic)
		std::cout << "Index not in wave bank!" << std::endl;
	else
	{
		this->instanceMusic->SetVolume(this->musicVolume);
		this->instanceMusic->Play(true);
	}
}

void SoundHandler::PlayAmbient(const int& index)
{
	this->instanceAmbient = this->waveBankAmbient->CreateInstance(index);
	if (!this->instanceAmbient)
		std::cout << "Index not in wave bank!" << std::endl;
	else
	{
		this->instanceAmbient->SetVolume(this->ambientVolume);
		this->instanceAmbient->Play(true);
	}
}

void SoundHandler::SetPitch(const float& newPitch)
{
	this->instanceAmbient->SetPitch(newPitch);
	this->instanceFX->SetPitch(newPitch);
	this->instanceMusic->SetPitch(newPitch);
}

void SoundHandler::SuspendAudio()
{
	this->audEngine->Suspend();
}

void SoundHandler::ResumeAudio()
{
	this->audEngine->Resume();
}

void SoundHandler::StopAudio()
{
	this->instanceAmbient->Stop();
	this->instanceFX->Stop();
	this->instanceMonster->Stop();
	this->instanceMusic->Stop();
}

void SoundHandler::MenuAudio()
{
	this->instanceFX->Stop();
	this->instanceMonster->Stop();
}
