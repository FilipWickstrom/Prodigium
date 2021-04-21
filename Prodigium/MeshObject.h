#pragma once
#include <string>

#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"

class MeshObject : public GameObject
{
private:
	Mesh mesh;

	//ID3D11ShaderResourceView* normalMapResourceView;
	//ID3D11ShaderResourceView* diffuseMapResourceView;

	bool isPickUp;
	bool isVisible;

public:

	MeshObject();
	virtual ~MeshObject();

	bool Initialize(std::string meshObject);

	void SetVisible(bool toggle = true);
	void SetPickUp(bool toggle = true);
	void Render();
};