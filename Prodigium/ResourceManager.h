#pragma once
#include <unordered_map>
#include <iostream>
#include <memory>
#include "UsefulStructuresHeader.h"
#include "UsefulHeader.h"
#include "Texture.h"

// The resourceManager will manage all the resources(Assets) in the game. 
// Remember to inherit from Resource.h for every type of asset. 
// Every Resource needs to know what ResourceType it is in its constructor
// Example Below:
// Test()
// :Resource(ResourceType::TEXTURE)
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

	void AddResource(std::string key, Resource* resource);

	ID3D11Texture2D* GetTextureInternal(const std::string& key);

	//Mesh* GetMeshInternal(const std::string& key)
	//{

	//	auto found = instance->meshes.find(key);

	//	if (found == instance->meshes.end())
	//	{
	//		Mesh* mesh = nullptr;
	//		if (!AddResource(key, mesh = new Mesh))
	//		{
	//			throw "ResourceManager: Couldn't add asset!\n";
	//		}

	//		return mesh;
	//	}

	//	return found->second;
	//}

protected:
	UINT referenceCount;

public:
	DELETE_COPY_ASSIGNMENT(ResourceManager);

	static void Initialize();

	static ID3D11Texture2D* GetTexture(const std::string& key)
	{
		ID3D11Texture2D* rv = instance->GetTextureInternal(key);

		if (rv == nullptr)
		{
			std::cout << "Resource is nullptr!" << std::endl;
		}

		return rv;
	}

	//static ID3D11Texture2D* GetMesh(const std::string& key)
	//{
	//	ID3D11Texture2D* rv = instance->GetMeshInternal(key);

	//	return rv;
	//}

	static UINT GetReferenceCount();

	static void Destroy();
};