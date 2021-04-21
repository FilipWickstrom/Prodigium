#include "Scene.h"

Scene::Scene()
{
	this->currentObject = 0;
}

Scene::~Scene()
{
}

void Scene::Add(ID3D11Device*& device, std::string filePath, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	/*
		Create a new MeshObject from input.
	*/

	MeshObject* newObject = new MeshObject;
	newObject->BuildMatrix(device, pos, scale, rotation);
	//newObject->Initialize(device, filePath + ".obj");

	this->objects.push_back(newObject);
	this->currentObject = (int)objects.size() - 1;
}

void Scene::UpdateMatrix(ID3D11Device*& device, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	/*
		Update the Matrix with input.
	*/


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
}

int Scene::GetNumberOfObjects() const
{
	return (int)this->objects.size();
}

void Scene::Render(ID3D11DeviceContext*& context)
{
	for (int i = 0; i < (int)this->objects.size(); i++)
	{
		this->objects[i]->Render(context);
	}
}

