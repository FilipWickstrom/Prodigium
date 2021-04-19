#pragma once

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

	ResourceType GetType()const;

	virtual bool Load() = 0;
};