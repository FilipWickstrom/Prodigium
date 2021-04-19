#include "Scene.h"

Scene::Scene()
{
	// Start by clearing the vector space.
	objects.clear();
}

Scene::~Scene()
{
	for (int i = 0; i < (int)objects.size(); i++)
	{
		objects.pop_back();
	}
}

void Scene::Add(ID3D11Device*& device, std::string filePath, DirectX::XMFLOAT3 pos,
	DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	/*
		Create a new object to be placed within the scene.
	*/
	MeshObject newObject;
	newObject.BuildMatrix(device, pos, scale, rotation);
	
	/*
		To be changed later on.
	*/
	newObject.LoadMesh(device, filePath);
	//newObject.LoadNormalTexture(device, filePath + "normal");
	//newObject.LoadDiffuseTexture(device, filePath + "diffuse");

	objects.push_back(&newObject);
}

void Scene::Remove(int index)
{
	objects.erase(objects.begin() + index);
}

void Scene::Render(ID3D11DeviceContext*& context)
{
	for (int i = 0; i < (int)objects.size(); i++)
	{
		objects[i]->Render(context);
	}
}
