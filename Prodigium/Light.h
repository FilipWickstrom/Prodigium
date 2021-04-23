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

	Light(LightType type);
	virtual ~Light();

	/*
		Initalizes the buffer as a directional light
	*/
	bool MakeDirectional(DirectionalLight dirL);
	bool MakeSpotLight(SpotLight spot);
	bool MakePointLight(PointLight point);

	ID3D11Buffer*& GetBuffer();
};

