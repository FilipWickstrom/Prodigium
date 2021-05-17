#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	this->audEngine = nullptr;
	this->soundEffect = nullptr;
}

SoundHandler::~SoundHandler()
{
	delete this->soundEffect;
	delete this->audEngine;
}

const bool SoundHandler::Initialize()
{
	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return false;

	DirectX::AUDIO_ENGINE_FLAGS eFlags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
	eFlags |= DirectX::AudioEngine_Debug;
#endif
	audEngine = new DirectX::AudioEngine( eFlags );

	return true;
}

void SoundHandler::Update()
{
	if (!this->audEngine->Update())
	{
		this->audEngine->Update();

		if (this->audEngine->IsCriticalError())
		{
			std::cerr << "Critical Error Occurred with Audio Device!" << std::endl;
		}
	}
}

void SoundHandler::PlayOneShot(const wchar_t* fileName)
{
	wchar_t filePath[50] = L"media/";
	wcscat_s(filePath, fileName);
	
	this->soundEffect = new DirectX::SoundEffect(this->audEngine, filePath);
	this->soundEffect->Play();
}

void SoundHandler::PlayLooping(const wchar_t* fileName, const bool& play3D, const DirectX::SimpleMath::Vector3& listnerPos, const DirectX::SimpleMath::Vector3& emitterPos)
{
	wchar_t filePath[50] = L"media/";
	wcscat_s(filePath, fileName);

	this->soundEffect = new DirectX::SoundEffect(this->audEngine, filePath);
	
	if (!play3D)
	{
		auto effect = this->soundEffect->CreateInstance();
		effect->Play(true);
	}
	
	else
	{
		auto effect = this->soundEffect->CreateInstance(DirectX::SoundEffectInstance_Use3D);

		DirectX::AudioListener listener;
		listener.SetPosition(listnerPos);
		DirectX::AudioEmitter emitter;
		emitter.SetPosition(emitterPos);

		effect->Apply3D(listener, emitter);
		effect->Play(true);
	}
}

void SoundHandler::PlayLooping(const wchar_t* fileName)
{
	wchar_t filePath[50] = L"media/";
	wcscat_s(filePath, fileName);
	this->soundEffect = this->soundEffect = new DirectX::SoundEffect(this->audEngine, filePath);
	auto effect = this->soundEffect->CreateInstance();
	effect->Play(true);
}
