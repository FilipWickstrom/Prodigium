#pragma once
#include <unordered_map>
#include "Resource.h"
#include <string>
#include <iostream>
#include <memory>
#include "UsefulStructuresHeader.h"

// The resourceManager will manage all the resources(Assets) in the game. 
// Remember to inherit from Resource.h for every type of asset. 
// Every Resource needs to know what ResourceType it is in its constructor
// Example Below:
// Test()
// :Resource(ResourceType::TEXTURE)
// Using GetResource() will give you the requested resource. 
// If it doesn't exist it will load it in otherwise it will directly return it.

class ResourceManager
{
private:
	std::unordered_map<std::string, Resource*> textures;
	std::unordered_map<std::string, Resource*> meshes;
	std::unordered_map<std::string, Resource*> audio;

	ResourceManager();
	virtual ~ResourceManager();

	static ResourceManager* instance;

	bool AddResource(std::string key, Resource* resource);

	//template <typename T>
	//T* GetTexture(const std::string& key)
	//{
	//	auto found = instance->textures.find(key);

	//	if (found == instance->textures.end())
	//	{
	//		Texture* texture = nullptr;

	//		if (!AddResource(key, texture = new Texture()))
	//		{
	//			throw "ResourceManager: Couldn't add asset!\n";
	//		}

	//		return dynamic_cast<T*>(texture);
	//	}
	//	return dynamic_cast<T*>(found->second);
	//}

	//template <typename T>
	//T* GetMesh(const std::string& key)
	//{

	//	auto found = instance->meshes.find(key);

	//	if (found == instance->meshes.end())
	//	{
	//		Mesh* mesh = nullptr;
	//		if (!AddResource(key, mesh = new Mesh))
	//		{
	//			throw "ResourceManager: Couldn't add asset!\n";
	//		}

	//		return dynamic_cast<T*>(mesh);
	//	}

	//	return dynamic_cast<T*>(found->second);
	//}

protected:
	UINT referenceCount;

public:
	DELETE_COPY_ASSIGNMENT(ResourceManager);

	static bool Initialize();

	// GetResource return a pointer to a resource
	//template <typename T>
	//T* GetResource(const std::string& key)
	//{
	//	T* rv = nullptr;

	//	if (std::is_same<T, Texture>::value)
	//	{
	//		rv = instance->GetTexture<T>(key);
	//	}
	//	//else if (std::is_same<T, Mesh>::value)
	//	//{
	//	//	rv = instance->GetMesh<T>(key);
	//	//}
	//	//else if (std::is_same<T, Audio>::value)
	//	//{
	//	//	rv = instance->GetAudio<T>(key);
	//	//}
	//	else
	//	{
	//		throw "Type is not supported!\n";
	//	}

	//	// More datatypes to add if we add more assets

	//	return rv;
	//}

	static ResourceManager* Instance()
	{
		if (instance == nullptr)
		{
			instance = new ResourceManager;
		}

		return instance;
	}

	const UINT GetReferenceCount() const;
};