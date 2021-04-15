#pragma once
#include "GameObject.h"
#include <string>

class MeshObject : public GameObject
{
private:

	UINT vertexCount;
	ID3D11Buffer* vertexBuffer;
	ID3D11Texture2D* diffuseMap;
	ID3D11Texture2D* normalMap;
	ID3D11ShaderResourceView* normalMapResourceView;
	ID3D11ShaderResourceView* diffuseMapResourceView;

	bool isPickUp;
	bool isVisible;

public:

	MeshObject();
	virtual ~MeshObject();

	void SetVisible(bool toggle = true);
	void SetPickUp(bool toggle = true);
	bool LoadMesh(ID3D11Device* device, std::string filePath);
	bool LoadDiffuseTexture(ID3D11Device* device, std::string filePath);
	bool LoadNormalTexture(ID3D11Device* device, std::string filePath);
	void Render(ID3D11DeviceContext*& context);
};