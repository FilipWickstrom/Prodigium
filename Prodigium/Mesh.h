#pragma once
#include <d3d11.h>
#include <iostream>
#include <vector>

#include "UsefulStructuresHeader.h"
#include "Resource.h"

#pragma warning(push, 0)
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#pragma warning(pop)
class Mesh : public Resource
{
private:
	std::vector<ID3D11Buffer*>vertexBuffers;
	std::vector<ID3D11Buffer*>indexBuffers;
	std::vector<UINT>indexCount;

public:
	//Will be used to create colliders
	std::vector<std::vector<DirectX::SimpleMath::Vector3>> meshPositions;
	
private:
	bool CreateVertIndiBuffers(const std::vector<Vertex>& vertices, const std::vector<UINT>& indices, UINT nrOfIndices);

public:
	Mesh();
	~Mesh();

	SphereCollider modelColliderSphere;
	
	bool LoadFile(std::string filename);
	void Render();
};