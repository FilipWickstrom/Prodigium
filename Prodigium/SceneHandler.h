#pragma once
#include "Scene.h"
class SceneHandler
{
private:

	std::vector<Scene> scenes;

public:

	SceneHandler();
	virtual ~SceneHandler();

	// Adds a new fresh scene into the vector<> call this when you want to init a new scene.
	void AddScene();

	// removes the scene at the indexed begin() + index spot.
	void RemoveScene(int index);

	// Get access to the indexed scene to edit it.
	Scene& EditScene(int index);

	// return the number of Scenes currently existing in the game.
	int GetNrOfScenes() const;

	// Render the scene with the sceneIndex.
	void Render(ID3D11DeviceContext*& context, int sceneIndex);
};