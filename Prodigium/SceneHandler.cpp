#include "SceneHandler.h"

SceneHandler::SceneHandler()
{
	this->AddScene();
}

SceneHandler::~SceneHandler()
{
}

void SceneHandler::AddScene()
{
	Scene newScene;
	scenes.push_back(newScene);
	this->currentScene = (int)scenes.size() - 1;
}

void SceneHandler::RemoveScene(int index)
{
	this->currentScene = 0;
	this->scenes.erase(this->scenes.begin() + index);
}

Scene& SceneHandler::EditScene()
{
	return this->scenes[this->currentScene];
}

void SceneHandler::SwitchScene(int index)
{
	if (index < (int)scenes.size() && index >= 0)
	{
		this->currentScene = index;
	}
	else
	{
		printf("You went too far %d is above %d\n", index, (int)scenes.size() - 1);
	}
}

int SceneHandler::GetNrOfScenes() const
{
	return (int)scenes.size();
}

void SceneHandler::Render(ID3D11DeviceContext*& context)
{
	this->scenes[this->currentScene].Render(context);
}
