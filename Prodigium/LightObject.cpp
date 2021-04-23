#include "LightObject.h"

LightObject::LightObject()
{
}

LightObject::~LightObject()
{
	for (int i = 0; i < (int)lights.size(); i++)
	{
		if (this->lights[i])
			delete this->lights[i];
	}
}

void LightObject::AddSpotLight(SpotLight spotL)
{
	Light* newL = new Light(LightType::SPOTLIGHT);
	if (!newL->MakeSpotLight(spotL))
	{

	}
}

void LightObject::AddDirLight(DirectionalLight dirL)
{
	Light* newL = new Light(LightType::DIRECTIONAL);
	if (!newL->MakeDirectional(dirL))
	{

	}
}

void LightObject::AddPointLight(PointLight pointL)
{
	Light* newL = new Light(LightType::POINTLIGHT);
	if (!newL->MakePointLight(pointL))
	{

	}
}

void LightObject::Render()
{
	/*
		Firstly check if any lights exist then render any of the existing lights.
	*/
	if ((int)lights.size() > 0)
	{
		for (int i = 0; i < (int)lights.size(); i++)
		{

		}
	}
}
