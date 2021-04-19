#include "SceneHandler.h"

void SceneHandler::ForestScene(ID3D11Device*& device)
{
	/*
		Create all the scene objects in here with the help of Scene class.
	*/
	Scene forestLevel;

	// Example way to models.
	forestLevel.Add(device, "Models/necklace_OBJ.obj");
	forestLevel.Add(device, "Models/drawing_OBJ.obj", { 5.0f, 1.0f, 25.0f });

	scenes.push_back(&forestLevel);
}

SceneHandler::SceneHandler()
{

}

SceneHandler::~SceneHandler()
{
	// Empty the Scenes.
	for (int i = 0; i < (int)scenes.size(); i++)
	{
		scenes.pop_back();
	}
}

void SceneHandler::Render(ID3D11DeviceContext*& context, int sceneIndex)
{
	scenes[sceneIndex]->Render(context);
}
