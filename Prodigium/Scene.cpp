#include "Scene.h"

Scene::Scene()
{

}

Scene::~Scene()
{
}

void Scene::Add(ID3D11Device*& device, std::string filePath, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale)
{
	MeshObject newObject;
	newObject.BuildMatrix(device, pos, scale, rotation);
	newObject.Initialize(device, filePath + ".obj");

	objects.push_back(&newObject);
}

void Scene::Remove(int index)
{
	objects.erase(objects.begin() + index);
}

int Scene::GetNumberOfObjects() const
{
	return (int)objects.size();
}

void Scene::Render(ID3D11DeviceContext*& context)
{
	for (int i = 0; i < (int)objects.size(); i++)
	{
		objects[i]->Render(context);
	}
}
