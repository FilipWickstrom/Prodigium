#pragma once
#include "Scene.h"
class SceneHandler
{
private:

	std::vector<Scene*> scenes;

public:

	SceneHandler();
	virtual ~SceneHandler();

	void AddScene();

	Scene& EditScene(int index);

	int GetNrOfScenes() const;

	void Render(ID3D11DeviceContext*& context, int sceneIndex);
};