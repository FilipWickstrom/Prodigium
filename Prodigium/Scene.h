#pragma once
#include <d3d11.h>
#include <vector>
#include <string>
#include <DirectXMath.h>
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

	// add only the name of the file in filePath since "Models/" and ".obj" will be added in the function.
	void Add(std::string objFile, std::string diffuseTxt = "", std::string normalTxt = "", 
			XMFLOAT3 position = {0.0f, 0.0f, 0.0f}, XMFLOAT3 rotation = {0.0f, 0.0f, 0.0f}, XMFLOAT3 scale = {1.0f, 1.0f, 1.0f});

	// update the object matrix buffer of object, as in update the position, rotation and scale.
	void UpdateMatrix(DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

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