#include "LightObject.h"

LightObject::LightObject()
{
}

LightObject::~LightObject()
{
}

void LightObject::AddLight(LightStruct& L)
{
	this->lights.push_back(L);
}

std::vector<LightStruct> LightObject::GetLights() const
{
	return this->lights;
}