#pragma once
#include <vector>
#include "Light.h"
class LightObject
{
private:

	// Flytta till sina egna klasser
	std::vector<Light*> lights;

public:

	LightObject();
	virtual ~LightObject();

	// adds a spot light into the scene at input position, pointing towards input.
	void AddSpotLight(SpotLight spotL);

	// adds a directional light into the scene that simply needs a direction. for use of e.g moon light.
	void AddDirLight(DirectionalLight dirL);

	// adds a point light into the scene that lights up in a box type fashion lighting up all sides.
	void AddPointLight(PointLight pointL);

	// render any type of lighting that may exist.
	void Render();
};

