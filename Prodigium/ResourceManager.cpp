#include "ResourceManager.h"

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
{
	this->textures = {};
	this->meshes = {};
	this->audio = {};
	this->referenceCount = 0;
}

ResourceManager::~ResourceManager()
{
}

bool ResourceManager::Initialize()
{
	return false;
}

bool ResourceManager::AddResource(std::string key, Resource* resource)
{
	if (resource == nullptr)
	{
		return false;
	}

	switch (resource->GetType())
	{
	case ResourceType::TEXTURE:
		this->textures.emplace(key, resource);
		break;
	case ResourceType::MESH:
		this->meshes.emplace(key, resource);
		break;
	case ResourceType::AUDIO:
		this->audio.emplace(key, resource);
		break;
	default:
		return false;
	}

	this->referenceCount++;

	return true;
}

const UINT ResourceManager::GetReferenceCount() const
{
	return this->referenceCount;
}
