#include "Scene.h"
using namespace DirectX::SimpleMath;
Scene::Scene()
{
	this->currentObject = 0;
}

Scene::~Scene()
{
	// Delete the allocated memory from vector.
	for (int i = 0; i < (int)objects.size(); i++)
	{
		delete this->objects[i];
	}
}

void Scene::Add(std::string objFile, std::string diffuseTxt, std::string normalTxt,
	Vector3 position, Vector3 rotation, Vector3 scale)
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
#ifdef _DEBUG
		std::cout << "Failed to add object" << std::endl;
#endif
	}
}

void Scene::Add(MeshObject* object)
{
	if (object != nullptr)
	{
		this->objects.push_back(object);
		this->currentObject = (int)objects.size() - 1;
	}
}

void Scene::UpdateMatrix(Vector3 pos, Vector3 rotation, Vector3 scale)
{
	/*
		Update the Matrix with input.
	*/
	if (!this->objects[this->currentObject]->UpdateMatrix(pos, scale, rotation))
	{
#ifdef _DEBUG
		std::cout << "Error lol noob" << "\n";
#endif
	}

#ifdef _DEBUG
	std::cout << "Matrix was updated for object on index " << this->currentObject << ". \n";
#endif

}

void Scene::Remove(int index)
{
	if (index < (int)this->objects.size() - 1 && index >= 0)
	{
		if (this->objects[index] != nullptr)
		{
			this->currentObject = 0;
			delete this->objects[index];
			this->objects.erase(objects.begin() + index);
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
		std::cout << "index was: " << index << ". Scope is: " << 0 << " to " << (int)this->objects.size() - 1 << "\n";
#endif
	}
}

void Scene::SwitchObject(int index)
{
	if (index < (int)this->objects.size() && index >= 0)
		this->currentObject = index;
	else
	{
#ifdef _DEBUG
		std::cout << "index is outside of vector scope!\n";
#endif
	}
}

int Scene::GetNumberOfObjects() const
{
	return (unsigned int)this->objects.size();
}

void Scene::RemoveAllObjects()
{
	for (int i = 0; i < (int)objects.size(); i++)
	{
		delete this->objects[(unsigned int)this->objects.size() - 1];
		this->objects.pop_back();
	}
	this->Pop();
}

void Scene::Pop()
{
	if ((int)this->objects.size() > 0)
	{
		delete this->objects[(unsigned int)this->objects.size() - 1];
		this->objects.pop_back();
	}
}

void Scene::Render()
{
	if ((int)this->objects.size() > 0)
	{
		for (int i = 0; i < (int)this->objects.size(); i++)
		{
			this->objects[i]->Render();
		}
	}
}

