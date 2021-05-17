#include "ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

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
	for (auto it = this->textures.begin(); it != this->textures.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
		}
	}
	this->textures.clear();
	for (auto it = this->meshes.begin(); it != this->meshes.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
		}
	}
	for (auto it = this->audio.begin(); it != this->audio.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
		}
	}
}

void ResourceManager::Initialize()
{
	if (!ResourceManager::instance)
	{
		ResourceManager::instance = new ResourceManager();
		Texture* gBufferTextures[BUFFER_COUNT];
		Texture* lightImage = new Texture();
		std::string key = "";

		for (int i = 0; i < BUFFER_COUNT; i++)
		{
			key = std::string(std::to_string(i));
			gBufferTextures[i] = new Texture();
			gBufferTextures[i]->Initialize(key);
			ResourceManager::instance->textures.emplace(key, gBufferTextures[i]);
			ResourceManager::instance->referenceCount++;
		}
		key = "FinalImage";
		lightImage->Initialize(key);
		ResourceManager::instance->textures.emplace(key, lightImage);
		ResourceManager::instance->referenceCount++;
	}
	else
	{
		std::cout << "ResourceManager is already initialized!" << std::endl;
	}
}

ID3D11Texture2D* ResourceManager::GetTexture(const std::string& key)
{
	ID3D11Texture2D* rv = instance->GetTextureInternal(key);

	if (rv == nullptr)
	{
		std::cout << "Resource is nullptr!" << std::endl;
	}

	return rv;
}

Mesh* ResourceManager::GetMesh(const std::string& key)
{
	Mesh* rv = instance->GetMeshInternal(key);

	if (rv == nullptr)
	{
		std::cout << "Resource is nullptr!" << std::endl;
	}

	return rv;
}

void ResourceManager::AddResource(std::string key, Resource* resource)
{
	if (resource == nullptr)
	{
		return;
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
		return;
	}

	this->referenceCount++;
}

ID3D11Texture2D* ResourceManager::GetTextureInternal(const std::string& key)
{
	auto found = instance->textures.find(key);

	if (found == instance->textures.end())
	{
		Texture* texture = new Texture();

		int width;
		int height;
		int comp;

		unsigned char* image = stbi_load(key.c_str(), &width, &height, &comp, STBI_rgb_alpha);

		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = (void*)image;

		// Distances in bytes of one line beginning to the next line = width * channels(RGBA)
		data.SysMemPitch = static_cast<UINT>(width * 4);
		// Used in 3D textures, default to 0
		data.SysMemSlicePitch = 0;

		if (!texture->Initialize(key, width, height, &data))
		{
			delete texture;
			
			return nullptr;
		}

		AddResource(key, texture);

		return texture->GetTexture2D();
	}

	return dynamic_cast<Texture*>(found->second)->GetTexture2D();
}

Mesh* ResourceManager::GetMeshInternal(const std::string& key)
{
	auto found = instance->meshes.find(key);

	if (found == instance->meshes.end())
	{
		Mesh* mesh = new Mesh();
		if (!mesh->LoadFile(key))
		{
			delete mesh;
			return nullptr;
		}

		AddResource(key, mesh);

		return mesh;
	}

	return dynamic_cast<Mesh*>(found->second);
}

const UINT ResourceManager::GetReferenceCount()
{
	return ResourceManager::instance->referenceCount;
}

void ResourceManager::Destroy()
{
	if (ResourceManager::instance)
	{
		delete ResourceManager::instance;
	}
}

void ResourceManager::AddCamera(std::string key, CameraObject* toAdd)
{
	if (toAdd)
	{
		ResourceManager::instance->cameras.emplace(key, toAdd);
	}
	else
	{
		throw("RM: Add Camera was nullptr!");
	}
}

const CameraObject* ResourceManager::GetCamera(const std::string& key)
{
	auto found = instance->cameras.find(key);

	if (found == instance->cameras.end())
	{
		throw("RM: Camera returned was nullptr!");
	}

	return found->second;
}