#include "SceneHandler.h"

SceneHandler::SceneHandler()
{

}

SceneHandler::~SceneHandler()
{
	// Empty
}

void SceneHandler::AddScene()
{
	Scene newScene;
	scenes.push_back(&newScene);
}

Scene& SceneHandler::EditScene(int index)
{
	return *this->scenes[index];
}

int SceneHandler::GetNrOfScenes() const
{
	return (int)scenes.size();
}

void SceneHandler::Render(ID3D11DeviceContext*& context, int sceneIndex)
{
	scenes[sceneIndex]->Render(context);
}
