#include "SoundHandler.h"

SoundHandler::SoundHandler()
{
	//this->audEngine = nullptr;
	this->soundEffect = nullptr;
}

SoundHandler::~SoundHandler()
{
	//this->audEngine.release();
	this->soundEffect.release();
}

const bool SoundHandler::Initialize()
{
	HRESULT hr;
	hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	if (FAILED(hr))
		return false;

	//std::unique_ptr<DirectX::AudioEngine> tempEngine;

	DirectX::AUDIO_ENGINE_FLAGS eFlags = DirectX::AudioEngine_Default;
#ifdef _DEBUG
	eFlags |= DirectX::AudioEngine_Debug;
#endif

	//tempEngine = std::make_unique<DirectX::AudioEngine>(eFlags);
	audEngine = std::make_unique<DirectX::AudioEngine>( eFlags );

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

//void SoundHandler::PlayOneShot(const std::string& fileName)
//{
//	this->soundEffect = std::make_unique<DirectX::SoundEffect>(this->audEngine.get(), fileName);
//	this->soundEffect->Play();
//}

void SoundHandler::PlayOneShot()
{
	this->soundEffect = std::make_unique<DirectX::SoundEffect>(this->audEngine.get(), L"media/ohYeah.wav");
	this->soundEffect->Play();
}

//void SoundHandler::PlayLooping(const std::string& fileName, const bool& play3D, const DirectX::SimpleMath::Vector3& listnerPos, const DirectX::SimpleMath::Vector3& emitterPos)
//{
//	this->soundEffect = std::make_unique<DirectX::SoundEffect>(this->audEngine.get(), fileName);
//	
//	if (!play3D)
//	{
//		auto effect = this->soundEffect->CreateInstance();
//		effect->Play();
//	}
//	
//	else
//	{
//		auto effect = this->soundEffect->CreateInstance(DirectX::SoundEffectInstance_Use3D);
//
//		DirectX::AudioListener listener;
//		listener.SetPosition(listnerPos);
//		DirectX::AudioEmitter emitter;
//		emitter.SetPosition(emitterPos);
//
//		effect->Apply3D(listener, emitter);
//		effect->Play();
//	}
//}
//
//void SoundHandler::PlayLooping(const std::string& fileName)
//{
//	//wchar_t filePath = L"/media/" + fileName;
//	this->soundEffect = std::make_unique<DirectX::SoundEffect>(this->audEngine.get(), fileName);
//	auto effect = this->soundEffect->CreateInstance();
//	effect->Play();
//}
