#pragma once
#include <unordered_map>
#include "Resource.h"
#include "Singleton.h"
#include <string>

class ResourceManager :public Singleton<ResourceManager>
{
private:
	std::unordered_map<std::string, Resource*> resources;
};