#include "SceneHandler.h"

SceneHandler::SceneHandler()
{

}

SceneHandler::~SceneHandler()
{
	for (int i = 0; i < (int)this->scenes.size(); i++)
	{
		this->scenes.pop_back();
	}
}

void SceneHandler::AddScene()
{
	Scene newScene;
	scenes.push_back(newScene);
}

void SceneHandler::RemoveScene(int index)
{
	this->scenes.erase(this->scenes.begin() + index);
}

Scene& SceneHandler::EditScene(int index)
{
	return this->scenes[index];
}

int SceneHandler::GetNrOfScenes() const
{
	return (int)scenes.size();
}

void SceneHandler::Render(ID3D11DeviceContext*& context, int sceneIndex)
{
	scenes[sceneIndex].Render(context);
}
