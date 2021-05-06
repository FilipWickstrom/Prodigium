#include "Mesh.h"
#include <iostream>

bool Mesh::CreateVertIndiBuffers(const std::vector<Vertex>& vertices, const std::vector<unsigned short>& indices, UINT nrOfIndices)
{
	/*-----Vertexbuffer-----*/
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(Vertex) * (UINT)vertices.size();
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

	/*-----Indexbuffer-----*/
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(unsigned short) * nrOfIndices;
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

bool Mesh::BuildColliders(const std::vector<Vertex>& vertices, const DirectX::XMFLOAT3& min, const DirectX::XMFLOAT3& max)
{
	DirectX::BoundingOrientedBox collider;

	collider.Center.x = (max.x + min.x) / 2.f;
	collider.Center.y = (max.y + min.y) / 2.f;
	collider.Center.z = (max.z + min.z) / 2.f;

	collider.Orientation = { 0.f, 1.f, 0.f, 0.f };
	collider.Extents.x = (max.x - min.x) / 2.f;
	collider.Extents.y = (max.y - min.y) / 2.f;
	collider.Extents.z = (max.z - min.z) / 2.f;

	colliders.push_back(collider);

	return true;
}

Mesh::Mesh()
	:Resource(ResourceType::MESH)
{
	this->objectFile = "";
}

Mesh::~Mesh()
{
	for (int i = 0; i < this->vertexBuffers.size(); i++)
	{
		this->vertexBuffers[i]->Release();
		this->indexBuffers[i]->Release();
	}
	this->vertexBuffers.clear();
	this->indexBuffers.clear();
	this->indexCount.clear();
}

bool Mesh::LoadFile(std::string filename)
{
	Assimp::Importer importer;

	//Load in the scene - can be many meshes together in one file
	const aiScene* scene = importer.ReadFile("Models/" + filename,
		aiProcess_Triangulate |               //Triangulate every surface
		aiProcess_JoinIdenticalVertices |     //Ignores identical veritices - memory saving
		aiProcess_FlipUVs |                   //Flips the textures to fit directX-style                                              
		aiProcess_FlipWindingOrder |          //Makes it clockwise order
		aiProcess_MakeLeftHanded);            //Use a lefthanded system for the models                                                                             
		//aiProcess_CalcTangentSpace);        //LATER FIX: can be added to fix tangents automatic

	//Check if it was possible to load file
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		return false;
	}

	//Going through all the meshes in the file
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		const aiMesh* mesh = scene->mMeshes[i];
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);

		DirectX::XMFLOAT3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
		DirectX::XMFLOAT3 max = { FLT_MIN, FLT_MIN, FLT_MIN };
		//Loading in all the vertices to the right structure
		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex temp;
			temp.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
			temp.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
			temp.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
			vertices.push_back(temp);

			min.x = std::min(min.x, temp.position.x);
			min.y = std::min(min.y, temp.position.y);
			min.z = std::min(min.z, temp.position.z);

			max.x = std::max(max.x, temp.position.x);
			max.y = std::max(max.y, temp.position.y);
			max.z = std::max(max.z, temp.position.z);
		}
		std::vector<unsigned short> indices;
		indices.reserve((size_t)mesh->mNumFaces * 3);
		UINT tempIndexCount = mesh->mNumFaces * 3;
		this->indexCount.push_back(tempIndexCount);

		//Loading in the indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace face = mesh->mFaces[i];
			//Only accepts 3 vertices per face
			if (face.mNumIndices == 3)
			{
				indices.push_back(face.mIndices[0]);
				indices.push_back(face.mIndices[1]);
				indices.push_back(face.mIndices[2]);
			}
		}

		//Vertex and index buffer to communicate with vertexshader later
		if (!CreateVertIndiBuffers(vertices, indices, tempIndexCount))
		{
			std::cout << "Failed to create vertex and index buffers..." << std::endl;
			return false;
		}

		this->BuildColliders(vertices, min, max);

		vertices.clear();
		indices.clear();
	}

	this->colliders.shrink_to_fit();
	this->collidersOriginal = this->colliders;

	std::cout << "Model: " << filename << " was successfully loaded! Meshes: " << scene->mNumMeshes << std::endl;
	return true;
}

void Mesh::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (int i = 0; i < this->vertexBuffers.size(); i++)
	{
		Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffers[i], &stride, &offset);
		Graphics::GetContext()->IASetIndexBuffer(this->indexBuffers[i], DXGI_FORMAT_R16_UINT, 0);
		Graphics::GetContext()->DrawIndexed(this->indexCount[i], 0, 0);
	}
}

void Mesh::RemoveColliders()
{
	this->colliders.clear();
	this->colliders.clear();
}