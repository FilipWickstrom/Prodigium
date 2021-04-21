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
#include <assimp/material.h>
#include <assimp/cimport.h>

class Mesh : public Resource
{
private:
	UINT indexCount;
	std::string objectFile;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

private:
	bool CreateVertIndiBuffers(std::vector<Vertex>& vertices, std::vector<unsigned short>& indices);

public:
	Mesh();
	~Mesh();

	bool LoadFile(std::string filename);
	void Render();
};