#include "Scene.h"

Scene::Scene()
{
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
	MeshObject newObject;
	newObject.BuildMatrix(device, pos, scale, rotation);

	objects.push_back(&newObject);
}

void Scene::Remove(int index)
{

}

void Scene::Render(ID3D11DeviceContext*& context)
{
	for (int i = 0; i < (int)objects.size(); i++)
	{
		objects[i]->Render(context);
	}
}
