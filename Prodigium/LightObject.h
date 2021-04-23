#pragma once
#include <vector>
#include "Light.h"
struct InfoStruct
{
	DirectX::XMFLOAT4 info;
};

class LightObject
{
private:

	// Flytta till sina egna klasser
	std::vector<Light*> lights;

	// Contains the amount of lights present in the scene.
	ID3D11Buffer* infoBuffer;

public:

	LightObject();
	virtual ~LightObject();

	// adds a spot light into the scene at input position, pointing towards input.
	void AddLight(LightStruct L);

	// render any type of lighting that may exist.
	void Render();
};

