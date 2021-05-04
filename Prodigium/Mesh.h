#pragma once
#include <d3d11.h>
#include <iostream>
#include <vector>

#include "UsefulStructuresHeader.h"
#include "Resource.h"
#include "Graphics.h"

//Assimp
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

class Mesh : public Resource
{
private:
	std::vector<ID3D11Buffer*>vertexBuffers;
	std::vector<ID3D11Buffer*>indexBuffers;
	std::vector<UINT>indexCount;

private:
	bool CreateVertIndiBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned short>& indices, UINT nrOfIndices);

public:
	Mesh();
	~Mesh();

	bool LoadFile(std::string filename);
	void Render();
};