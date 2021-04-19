#pragma once
#include "Scene.h"
class SceneHandler
{
private:

	std::vector<Scene*> scenes;

	// Initializer for Forest Scene/Level
	void ForestScene(ID3D11Device*& device);

public:

	SceneHandler();
	virtual ~SceneHandler();

	void Render(ID3D11DeviceContext*& context, int sceneIndex);
};