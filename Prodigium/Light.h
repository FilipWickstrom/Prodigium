#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"

enum class LightType
{
	DIRECTIONAL,
	SPOTLIGHT,
	POINTLIGHT,

	COUNT,
	NONE
};

class Light
{
private:

	LightType type;
	ID3D11Buffer* lightBuffer;

public:

	Light();
	virtual ~Light();

	/*
		Initalizes the buffer as a directional light
	*/
	bool MakeLight(LightStruct L);

	ID3D11Buffer*& GetBuffer();
};

