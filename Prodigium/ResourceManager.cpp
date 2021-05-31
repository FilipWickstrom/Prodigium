#include "ResourceManager.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

ResourceManager* ResourceManager::instance = nullptr;

ResourceManager::ResourceManager()
{
	this->textures = {};
	this->meshes = {};
	this->audio = {};
	this->animatedObjects = {};
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
	this->meshes.clear();

	for (auto it = this->audio.begin(); it != this->audio.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
		}
	}
	this->cameras.clear();

	for (auto it = this->animatedObjects.begin(); it != this->animatedObjects.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
		}
	}
	this->animatedObjects.clear();
}

void ResourceManager::Initialize()
{
	if (!ResourceManager::instance)
	{
		ResourceManager::instance = new ResourceManager();
	}
	else
	{
		std::cout << "ResourceManager is already initialized!" << std::endl;
	}
}

ID3D11Texture2D* ResourceManager::GetTexture(const std::string& key, bool isGBufferTexture)
{
	ID3D11Texture2D* rv = instance->GetTextureInternal(key, isGBufferTexture);

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
	case ResourceType::ANIMATEDOBJ:
		this->animatedObjects.emplace(key, resource);
		break;
	default:
		return;
	}

	this->referenceCount++;
}

ID3D11Texture2D* ResourceManager::GetTextureInternal(const std::string& key, bool isGBufferTexture)
{
	auto found = instance->textures.find(key);

	if (found == instance->textures.end())
	{
		Texture* texture = new Texture();

		if (!isGBufferTexture)
		{
			int width;
			int height;
			int comp;

			unsigned char* image = stbi_load(key.c_str(), &width, &height, &comp, STBI_rgb_alpha);

			if (!image)
			{
				return nullptr;
			}

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
			stbi_image_free(image);
		}
		else
		{
			if (!texture->Initialize(key))
			{
				delete texture;

				return nullptr;
			}
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

AnimatedObject* ResourceManager::GetAnimObjIternal(const std::string& key)
{
	auto found = instance->animatedObjects.find(key);

	if (found == instance->animatedObjects.end())
	{
		AnimatedObject* animObj = new AnimatedObject();
		if (!animObj->Initialize(key))
		{
			delete animObj;
			return nullptr;
		}

		AddResource(key, animObj);

		return animObj;
	}

	return dynamic_cast<AnimatedObject*>(found->second);
}

const UINT ResourceManager::GetReferenceCount()
{
	return ResourceManager::instance->referenceCount;
}

void ResourceManager::Destroy()
{
	if (ResourceManager::instance != nullptr)
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

CameraObject* ResourceManager::GetCamera(const std::string& key)
{
	auto found = instance->cameras.find(key);

	if (found == instance->cameras.end())
	{
		throw("RM: Camera is not in ResourceManager!");
	}
	if (found->second == nullptr)
	{
		throw("RM: Camera is nullptr!");
	}

	return found->second;
}

void ResourceManager::RemoveCamera(std::string key)
{
	auto found = instance->cameras.find(key);

	if (found == instance->cameras.end())
	{
		throw("RM: Camera is not in ResourceManager!");
	}
	else
	{
		delete instance->cameras[key];
		instance->cameras.erase(key);
	}
}

AnimatedObject* ResourceManager::GetAnimateObject(std::string key)
{
	AnimatedObject* rv = instance->GetAnimObjIternal(key);

	if (rv == nullptr)
	{
		std::cout << "Resource is nullptr!" << std::endl;
	}

	return rv;
}
