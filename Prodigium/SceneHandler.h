#pragma once
#include "Scene.h"
#include "Graphics.h"

class SceneHandler
{
private:

	std::vector<Scene*> scenes;

	// "Points" Towards the currently selected scene.
	int currentScene;

public:

	/*
		Creates a handler for scenes within the game. Within is a "pointer" which takes care of which scene to call on
		EditScene() call, to change it simply call SwitchScene('wanted scene') to switch which scene is currently active.
		Remember on RemoveScene() that vector<> starts at spot 0. On declaration a new scene will be added to the vector<> so
		no need to call AddScene() at the beginning since ONE already exists.
	*/
	SceneHandler();
	virtual ~SceneHandler();

	// Adds a new fresh scene into the vector<> call this when you want to init a new scene. currentScene pointer will point towards the newely created scene.
	void AddScene();

	// removes the scene at the indexed begin() + index spot. This resets the currentScene pointer to first element.
	void RemoveScene(int index);

	// get access to the currentScene pointer which points to current selected scene.
	Scene& EditScene();

	// switch where the currentScene pointer is going to point.
	void SwitchScene(int index);

	// return the number of Scenes currently existing in the game.
	int GetNrOfScenes() const;

	// will cycle through all scenes and pop them from the vector.
	void RemoveAllScenes();

	// runs a simple pop_back if so is desired. Resets the currentScene to 0.
	void Pop();

	// render the objects in current selected scene. if you're in Game.h dont touch this!
	void Render();

	// render the lights in the current selected scene. if you're in Game.h dont touch this!
	void RenderLights();
};