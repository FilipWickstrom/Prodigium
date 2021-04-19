#include "Resource.h"

Resource::Resource(ResourceType type)
{
	this->type = type;
}

Resource::~Resource()
{
}

ResourceType Resource::GetType() const
{
	return this->type;
}
