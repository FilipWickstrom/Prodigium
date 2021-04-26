#include "SceneHandler.h"

SceneHandler::SceneHandler()
{
	this->AddScene();
}

SceneHandler::~SceneHandler()
{
	for (int i = 0; i < (int)this->scenes.size(); i++)
	{
		if(this->scenes[i])
			delete this->scenes[i];
	}
}

void SceneHandler::AddScene()
{
	Scene* newScene = new Scene;
	scenes.push_back(newScene);
	this->currentScene = (int)scenes.size() - 1;
}

void SceneHandler::RemoveScene(int index)
{
	if (index < (int)this->scenes.size() - 1 && index >= 0)
	{
		if (this->scenes[index] != nullptr)
		{
			this->currentScene = 0;
			delete this->scenes[index];
			this->scenes.erase(this->scenes.begin() + index);
		}
		else
		{
#ifdef _DEBUG
			std::cout << "Object was a nullptr!\n";
#endif
		}
	}
	else
	{
#ifdef _DEBUG

		std::cout << "index is outside of vector scope!\n";
		std::cout << "Index was: " << index << ". Scope is: " << 0 << " to " << (int)this->scenes.size() - 1 << "\n";
#endif
	}
}

Scene& SceneHandler::EditScene()
{
	return *this->scenes[this->currentScene];
}

void SceneHandler::SwitchScene(int index)
{
	if (index < (int)scenes.size() - 1 && index >= 0)
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
	return (unsigned int)scenes.size();
}

void SceneHandler::RemoveAllScenes()
{
	for (int i = 0; i < (int)scenes.size(); i++)
	{
		delete this->scenes[(unsigned int)this->scenes.size() - 1];
		scenes.pop_back();
	}
}

void SceneHandler::Pop()
{
	if ((int)this->scenes.size() > 0)
	{
		this->currentScene = 0;
		delete this->scenes[(unsigned int)this->scenes.size() - 1];
		this->scenes.pop_back();
	}
}

void SceneHandler::Render()
{
	if ((int)this->scenes.size() > 0)
	{
		this->scenes[this->currentScene]->Render();
	}
}
