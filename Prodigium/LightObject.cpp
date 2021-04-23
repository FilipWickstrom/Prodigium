#include "LightObject.h"

LightObject::LightObject()
{
	this->infoBuffer = nullptr;
}

LightObject::~LightObject()
{
	for (int i = 0; i < (int)lights.size(); i++)
	{
		if (this->lights[i])
			delete this->lights[i];
	}

	if (this->infoBuffer)
		this->infoBuffer->Release();
}

void LightObject::AddLight(LightStruct L)
{
	Light* newL = new Light();
	if (newL->MakeLight(L))
	{
		lights.push_back(newL);
	}
	else
	{
#ifdef _DEBUG
		std::cout << "Loading light failed.\n";
#endif
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
