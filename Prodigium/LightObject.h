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
	std::vector<LightStruct> lights;

public:

	LightObject();
	virtual ~LightObject();

	// adds a spot light into the scene at input position, pointing towards input.
	void AddLight(LightStruct& L);

	std::vector<LightStruct> GetLights() const;
};

