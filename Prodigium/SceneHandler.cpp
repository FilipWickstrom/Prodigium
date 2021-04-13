#include "SceneHandler.h"

void SceneHandler::ForestScene()
{
	/*
		Create all the scene objects in here with the help of Scene class.
	*/
	Scene forestLevel;

	// Example way to add a tree.
	//forestLevel.Add("/objects/tree.obj", 10.0f, 1.0f, 10.0f, 0.0f);
}

SceneHandler::SceneHandler()
{

}

SceneHandler::~SceneHandler()
{
	// Empty
}

void SceneHandler::Render(ID3D11DeviceContext*& context, int sceneIndex)
{
	scenes[sceneIndex]->Render(context);
}
