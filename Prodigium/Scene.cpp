#include "Scene.h"

Scene::Scene()
{
	this->currentObject = 0;
}

Scene::~Scene()
{
}

void Scene::Add(std::string objFile, std::string diffuseTxt, std::string normalTxt,
	XMFLOAT3 position, XMFLOAT3 rotation, XMFLOAT3 scale)	
{
	/*
		Create a new MeshObject from input.
	*/
	MeshObject* newObject = new MeshObject;
	if (newObject->Initialize(objFile, diffuseTxt, normalTxt, position, rotation, scale))
	{
		this->objects.push_back(newObject);
		this->currentObject = (int)objects.size() - 1;
	}
	else
	{
		std::cout << "Failed to add object" << std::endl;
	}
}

void Scene::UpdateMatrix(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	/*
		Update the Matrix with input.
	*/
	this->objects[this->currentObject]->UpdateMatrix(pos, scale, rotation);

}

void Scene::Remove(int index)
{
	this->currentObject = 0;
	this->objects.erase(objects.begin() + index);
}

void Scene::SwitchObject(int index)
{
	if (index < (int)this->objects.size() && index >= 0)
		this->currentObject = index;

#ifdef _DEBUG

	std::cout << "Index is outside of the vector scope!" << "\n";

#endif
}

int Scene::GetNumberOfObjects() const
{
	return (int)this->objects.size();
}

void Scene::RemoveAllObjects()
{
	for (int i = 0; i < (int)objects.size(); i++)
	{
		this->objects.pop_back();
	}
}

void Scene::Pop()
{
	this->objects.pop_back();
}

void Scene::Render()
{
	for (int i = 0; i < (int)this->objects.size(); i++)
	{
		this->objects[i]->Render();
	}
}

