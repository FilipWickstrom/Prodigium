#include "Resource.h"

Resource::Resource(ResourceType type)
{
	this->type = type;
}

Resource::~Resource()
{
}

const ResourceType Resource::GetType() const
{
	return this->type;
}
