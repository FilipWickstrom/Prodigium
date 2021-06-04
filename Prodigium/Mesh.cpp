#include "Mesh.h"
#include <iostream>
#include "Graphics.h"

using namespace DirectX::SimpleMath;

bool Mesh::CreateVertIndiBuffers(const std::vector<MyFileFormat::VertexData>& vertices, const std::vector<UINT>& indices, UINT nrOfIndices)
{
	/*-----Vertexbuffer-----*/
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(MyFileFormat::VertexData) * (UINT)vertices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &vertices[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	ID3D11Buffer* vertbuffer;
	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &vertbuffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create vertex buffer..." << std::endl;
		return false;
	}
	this->vertexBuffers.push_back(vertbuffer);
	
	
	//-----Indexbuffer-----
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * nrOfIndices;
	data.pSysMem = &indices[0];

	ID3D11Buffer* indbuffer;
	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &indbuffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create index buffer..." << std::endl;
		return false;
	}
	this->indexBuffers.push_back(indbuffer);
	
	return true;
}

Mesh::Mesh()
	:Resource(ResourceType::MESH)
{
}

Mesh::~Mesh()
{

	for (int i = 0; i < this->vertexBuffers.size(); i++)
	{
		if (this->vertexBuffers[i])
			this->vertexBuffers[i]->Release();
		if (this->indexBuffers[i])
			this->indexBuffers[i]->Release();
	}
	this->vertexBuffers.clear();
	this->indexBuffers.clear();
	this->indexCount.clear();

	for (size_t i = 0; i < this->meshPositions.size(); i++)
	{
		this->meshPositions[i].clear();
	}
	this->meshPositions.clear();
}

bool Mesh::LoadFile(std::string fileName)
{
	std::string filePath = "Models/" + fileName;

	if (AssetLoader::LoadModel(filePath.c_str()))
	{
		
		int nrOfMeshes = AssetLoader::GetNumberOfMeshesInScene();

		if (nrOfMeshes <= 0)
			return false;
		
		this->meshes.resize(nrOfMeshes);
		meshPositions.resize(nrOfMeshes);

		this->meshes[0] = AssetLoader::GetModel(0);

		this->vertexSet.push_back(AssetLoader::GetVertexSet(0));
		this->nrOfVertices.push_back((UINT)this->meshes[0].nrOfVertices);
		std::vector<MyFileFormat::VertexData> vertices;
		std::vector<UINT> indices;
		indices.resize(meshes[0].nrOfVertices);
		UINT nrOfIndices = (UINT)indices.size();

		this->indexCount.push_back(nrOfIndices);

		vertices.resize(nrOfVertices[0]);
		meshPositions[0].resize(meshes[0].nrOfVertices);
		for (int i = 0; i < meshes[0].nrOfVertices; i++)
		{
			//Positions
			vertices[i].positions.x = vertexSet[0][i].positions.x;
			vertices[i].positions.y = vertexSet[0][i].positions.y;
			vertices[i].positions.z = vertexSet[0][i].positions.z;
			DirectX::SimpleMath::Vector3 meshPos = DirectX::SimpleMath::Vector3(vertices[i].positions.x, vertices[i].positions.y, vertices[i].positions.z);
			meshPositions[0].push_back(meshPos);

			//Normals
			vertices[i].normals.x = vertexSet[0][i].normals.x;
			vertices[i].normals.y = vertexSet[0][i].normals.y;
			vertices[i].normals.z = vertexSet[0][i].normals.z;

			//UVs
			vertices[i].uvs.x = vertexSet[0][i].uvs.x;
			vertices[i].uvs.y = vertexSet[0][i].uvs.y;

			//Tangents
			vertices[i].tangent.x = vertexSet[0][i].tangent.x;
			vertices[i].tangent.y = vertexSet[0][i].tangent.y;
			vertices[i].tangent.z = vertexSet[0][i].tangent.z;

			//BiTangents
			vertices[i].biTangent.x = vertexSet[0][i].biTangent.x;
			vertices[i].biTangent.y = vertexSet[0][i].biTangent.y;
			vertices[i].biTangent.z = vertexSet[0][i].biTangent.z;
		}


		//Reverse the draw order of triangle
		size_t o = 2;
		std::vector<MyFileFormat::VertexData> rewindOrder;
		for (int i = 0; i < meshes[0].nrOfVertices / 3; i++)
		{
			rewindOrder.push_back(vertices[o]);
			rewindOrder.push_back(vertices[o - 1]);
			rewindOrder.push_back(vertices[o - 2]);
			o += 3;
		}

		if (!CreateVertIndiBuffers(rewindOrder, indices, nrOfIndices))
			return false;

		if (meshes[0].hasTexture == 1)
			this->diffuseTextureName = meshes[0].textureName;
		if (meshes[0].hasNormalMap == 1)
			this->normalMapTextureName = meshes[0].normalMapName;
			
		return true;
	}
	else
		return false;		
}

void Mesh::Render()
{
	UINT stride = sizeof(MyFileFormat::VertexData);
	UINT offset = 0;
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	for (int i = 0; i < this->vertexBuffers.size(); i++)
	{
		Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffers[i], &stride, &offset);
		Graphics::GetContext()->Draw(this->nrOfVertices[i], 0);
	}
}