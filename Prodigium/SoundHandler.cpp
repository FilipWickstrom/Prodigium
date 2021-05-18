#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	this->audEngine = nullptr;
	this->soundEffect = nullptr;
	this->waveBank = nullptr;
	this->instance = nullptr;
	this->masterVolume = 1.f;
}

SoundHandler::~SoundHandler()
{
	this->instance.release();
	this->waveBank.release();
	this->soundEffect.release();
	this->audEngine.release();	
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
	this->waveBank = std::make_unique<DirectX::WaveBank>(this->audEngine.get(), L"media/prodigium.xwb");

	return true;
}

void SoundHandler::Update()
{
	if (!this->audEngine->Update())
	{
		std::cout << "No audio playing!" << std::endl;

		if (this->audEngine->IsCriticalError())
		{
			std::cerr << "Critical Error Occurred with Audio Device!" << std::endl;
		}
	}
	this->audEngine->Update();
}

void SoundHandler::SetVolume(const float& newVolume)
{
	this->masterVolume = newVolume;
}

void SoundHandler::PlayOneShot(const int& index)
{
	this->waveBank->Play(index, masterVolume, 0, 0);
}

void SoundHandler::PlayLooping(const int& index, const bool& use3D, const DirectX::SimpleMath::Vector3& listnerPos,
							   const DirectX::SimpleMath::Vector3& emitterPos)
{
	if (!use3D)
	{
		this->instance = this->waveBank->CreateInstance(index);
		if (!this->instance)
			std::cout << "Index not in wave bank!" << std::endl;
		else
		{
			this->instance->SetVolume(this->masterVolume);
			this->instance->Play(true);
		}
	}
	else
	{		
		this->instance = this->waveBank->CreateInstance(index, DirectX::SoundEffectInstance_Use3D | DirectX::SoundEffectInstance_ReverbUseFilters);
		if(!this->instance)
			std::cout << "Index not in wave bank!" << std::endl;
		else
		{
			DirectX::AudioListener listner;
			listner.SetPosition(listnerPos);
			DirectX::AudioEmitter emitter;
			emitter.SetPosition(emitterPos);

			this->instance->SetVolume(this->masterVolume);
			this->instance->Apply3D(listner, emitter);
			this->instance->Play(true);
		}
	}
}
