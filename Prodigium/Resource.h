#pragma once
#include <string>

enum class ResourceType
{
	TEXTURE,
	MESH,
	AUDIO
};

class Resource
{
private:
	ResourceType type;

public:
	Resource(ResourceType type);
	virtual ~Resource();

	const ResourceType GetType()const;
};