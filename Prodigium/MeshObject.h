#pragma once
#include "GameObject.h"
#include <string>

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

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

//Help functions
private:



public:

	MeshObject();
	virtual ~MeshObject();

	bool Initialize();

	//MAKE PRIVATE LATER:
	void SetVisible(bool toggle = true);
	void SetPickUp(bool toggle = true);
	bool LoadMesh(ID3D11Device* device, std::string filePath);
	bool LoadDiffuseTexture(ID3D11Device* device, std::string filePath);
	bool LoadNormalTexture(ID3D11Device* device, std::string filePath);
	
	//CAN BE PUBLIC:
	void Render(ID3D11DeviceContext*& context);
};