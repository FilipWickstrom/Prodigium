#pragma once
#include <d3d11.h>
#include <iostream>
#include <vector>

#include "UsefulStructuresHeader.h"
#include "Resource.h"

#include <assetloader/Assetloader.h>

#pragma warning(pop)
class Mesh : public Resource
{
private:
	std::vector<ID3D11Buffer*>vertexBuffers;
	std::vector<ID3D11Buffer*>indexBuffers;
	std::vector<UINT>indexCount;

	std::vector<MyFileFormat::Mesh> meshes;
	std::vector<MyFileFormat::Mesh> meshesColliders;
	std::vector<std::vector<MyFileFormat::VertexData>> vertexSet;
	std::vector<std::vector<MyFileFormat::VertexData>> vertexSetColliders;
	std::vector<UINT> nrOfVertices;
	std::vector<UINT> nrOfVerticesColliders;

public:
	// Mesh positions local space
	// Used for colliders and reserved for other computations
	std::vector<std::vector<DirectX::SimpleMath::Vector3>> meshPositions;

	std::string diffuseTextureName;
	std::string normalMapTextureName;
	
private:
	bool CreateVertIndiBuffers(const std::vector<MyFileFormat::VertexData>& vertices, const std::vector<UINT>& indices, UINT nrOfIndices);

public:
	Mesh();
	virtual ~Mesh();
	
	bool LoadFile(std::string fileName);
	void Render();
};