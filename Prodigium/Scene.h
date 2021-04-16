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

	void Add(ID3D11Device*& device, std::string filePath, DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), 
		DirectX::XMFLOAT3 rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3 scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f));
	void Remove(int index);
	void Render(ID3D11DeviceContext*& context);

};