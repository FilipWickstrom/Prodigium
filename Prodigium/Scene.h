#pragma once
#include <d3d11.h>
#include <vector>
#include <string>

class Scene
{
private:

	// Vector for all the objects present in this scene.

public:

	Scene();
	virtual ~Scene();

	void Add(std::string filePath, float x, float y, float z, float rotation);
	void Remove(int index);
	void Render(ID3D11DeviceContext*& context);

};