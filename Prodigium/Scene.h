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

public:

	Scene();
	virtual ~Scene();

	// add only the name of the file in filePath since "Models/" and ".obj" will be added in the function.
	void Add(ID3D11Device*& device, std::string filePath, DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 
		DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));

	// the vector will erase whatever item was at begin() + index
	void Remove(int index);

	// return the number of objects inside the scene.
	int GetNumberOfObjects() const;

	// loop through all objects and call their render function.
	void Render(ID3D11DeviceContext*& context);

};