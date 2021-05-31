#pragma once
#include <unordered_map>
#include <iostream>
#include <memory>
#include "UsefulStructuresHeader.h"
#include "UsefulHeader.h"
#include "Texture.h"
#include "Mesh.h"
#include "CameraObject.h"
#include "AnimatedObject.h"

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
	std::unordered_map<std::string, CameraObject*> cameras;
	std::unordered_map<std::string, Resource*> animatedObjects;

	ResourceManager();
	virtual ~ResourceManager();

	static ResourceManager* instance;

	void AddResource(std::string key, Resource* resource);

	ID3D11Texture2D* GetTextureInternal(const std::string& key, bool isGBufferTexture);
	Mesh* GetMeshInternal(const std::string& key);
	AnimatedObject* GetAnimObjIternal(const std::string& key);

protected:
	UINT referenceCount;

public:
	DELETE_COPY_ASSIGNMENT(ResourceManager);

	static void Initialize();

	static ID3D11Texture2D* GetTexture(const std::string& key, bool isGBufferTexture = false);

	static Mesh* GetMesh(const std::string& key);

	static const UINT GetReferenceCount();

	static void Destroy();

	static void AddCamera(std::string key, CameraObject* toAdd);
	
	static CameraObject* GetCamera(const std::string& key);

	static void RemoveCamera(std::string key);

	static AnimatedObject* GetAnimateObject(std::string key);
};