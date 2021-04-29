#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include "MeshObject.h"

class Scene
{
private:

	// Vector for all the objects present in this scene.
	std::vector<MeshObject*> objects;

	int currentObject;

public:

	Scene();
	virtual ~Scene();

	// adds an object to the scene, current selected object will point towards this new object.
	void Add(std::string objFile, std::string diffuseTxt = "", std::string normalTxt = "", 
		DirectX::SimpleMath::Vector3 position = {0.0f, 0.0f, 0.0f}, DirectX::SimpleMath::Vector3 rotation = {0.0f, 0.0f, 0.0f}, DirectX::SimpleMath::Vector3 scale = {1.0f, 1.0f, 1.0f});

	// Adds a reference to an already initialized object to the scene
	void Add(MeshObject* object);

	// update the object matrix buffer of current selected object, as in update the position, rotation and scale.
	void UpdateMatrix(DirectX::SimpleMath::Vector3 pos = { 0.0f, 0.0f, 0.0f },
		DirectX::SimpleMath::Vector3 rotation = { 0.0f, 0.0f, 0.0f }, DirectX::SimpleMath::Vector3 scale = { 1.0f, 1.0f, 1.0f });

	// the vector will erase whatever item was at begin() + index, resets currentObject to 0.
	void Remove(int index);

	// switches the to indexed object if it is within the scope of the vector! else nothing changes.
	void SwitchObject(int index);

	// return the number of objects inside the scene.
	int GetNumberOfObjects() const;

	// will cycle through vector and pop_back() through all elements.
	void RemoveAllObjects();

	// runs a simple pop_back if so is desired.
	void Pop();

	// loop through all objects and call their render function.
	void Render();
};