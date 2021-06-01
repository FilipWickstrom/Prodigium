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
	/*Assimp::Importer importer;

	//Load in the scene - can be many meshes together in one file
	const aiScene* scene = importer.ReadFile("Models/" + filename,
											aiProcess_Triangulate |               //Triangulate every surface
											aiProcess_JoinIdenticalVertices |     //Ignores identical veritices - memory saving
											aiProcess_FlipUVs |                   //Flips the textures to fit directX-style                                              
											aiProcess_FlipWindingOrder |          //Makes it clockwise order
											aiProcess_MakeLeftHanded |			  //Use a lefthanded system for the models                                                                             
											aiProcess_CalcTangentSpace);          //Fix tangents automatic for us

	//Check if it was possible to load file
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		importer.FreeScene();
		return false;
	}

	std::vector<DirectX::SimpleMath::Vector3> positions;

	//Going through all the meshes in the file
	for (unsigned int m = 0; m < scene->mNumMeshes; m++)
	{
		const aiMesh* mesh = scene->mMeshes[m];
		std::vector<Vertex> vertices;
		vertices.reserve(mesh->mNumVertices);
		positions.reserve(mesh->mNumVertices);

		//Load in material
		const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		float shiniess = 0.0f;
		aiColor4D specular = { 0.0f,0.0f,0.0f,0.0f };

		if (material != nullptr)
		{
			aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shiniess);
			aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
		}

		//Loading in all the vertices to the right structure
		for (unsigned int v = 0; v < mesh->mNumVertices; v++)
		{
			Vertex temp;
			temp.position = { mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z };
			temp.normal = { mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z };
			temp.uv = { mesh->mTextureCoords[0][v].x, mesh->mTextureCoords[0][v].y };
			temp.tangent = { mesh->mTangents[v].x, mesh->mTangents[v].y, mesh->mTangents[v].z };
			//Add material
			temp.specular = { specular.r, specular.g, specular.b, shiniess };
			vertices.push_back(temp);

			//Adding all the vertices positions to an temp vector
			positions.push_back(temp.position);
		}
		std::vector<UINT> indices;
		indices.reserve((size_t)mesh->mNumFaces * 3);
		UINT tempIndexCount = mesh->mNumFaces * 3;
		this->indexCount.push_back(tempIndexCount);

		//Add to the 2d-array of all the positions
		this->meshPositions.push_back(positions);
		positions.clear();

		//Loading in the indices
		for (unsigned int f = 0; f < mesh->mNumFaces; f++)
		{
			const aiFace face = mesh->mFaces[f];
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
			importer.FreeScene();
			return false;
		}

		vertices.clear();
		indices.clear();
	}

	std::cout << "Model: " << filename << " was successfully loaded! Meshes: " << scene->mNumMeshes << std::endl;
	importer.FreeScene();
	return true;
	*/
	std::string filePath = "Models/" + fileName;

	if (AssetLoader::LoadModel(filePath.c_str()))
	{
		int nrOfMeshes = AssetLoader::GetNumberOfMeshesInScene();

		if (nrOfMeshes <= 0)
			return false;

		this->meshes.resize(nrOfMeshes);

		if (nrOfMeshes == 1)
		{
			this->meshes[0] = AssetLoader::GetModel(0);
			this->vertexSet.insert({ 0, AssetLoader::GetVertexSet(0) });
			this->nrOfVertices.push_back((UINT)this->meshes[0].nrOfVertices);
			std::vector<MyFileFormat::VertexData> vertices;
			std::vector<UINT> indices;
			indices.resize(meshes[0].nrOfVertices);
			UINT nrOfIndices = indices.size();

			this->indexCount.push_back(nrOfIndices);

			vertices.resize(nrOfVertices[0]);
			for (int i = 0; i < meshes[0].nrOfVertices; i++)
			{
				//Positions
				vertices[i].positions.X = vertexSet[0][i].positions.X;
				vertices[i].positions.Y = vertexSet[0][i].positions.Y;
				vertices[i].positions.Z = -vertexSet[0][i].positions.Z; //Inverts the Z:axis to be left-handed

				//Normals
				vertices[i].normals.X = vertexSet[0][i].normals.X;
				vertices[i].normals.Y = vertexSet[0][i].normals.Y;
				vertices[i].normals.Z = -vertexSet[0][i].normals.Z;	//Inverts the Z:axis to be left-handed

				//UVs
				vertices[i].uvs.X = vertexSet[0][i].uvs.X;
				vertices[i].uvs.Y = -vertexSet[0][i].uvs.Y; //Inverts the V:value to comply to DirectX

				//Tangents
				vertices[i].tangent.X = vertexSet[0][i].tangent.X;
				vertices[i].tangent.Y = vertexSet[0][i].tangent.Y;
				vertices[i].tangent.Z = vertexSet[0][i].tangent.Z;

				//BiTangents
				vertices[i].biTangent.X = vertexSet[0][i].biTangent.X;
				vertices[i].biTangent.Y = vertexSet[0][i].biTangent.Y;
				vertices[i].biTangent.Z = vertexSet[0][i].biTangent.Z;
			}

			//Reverse the draw order of triangle
			size_t o = 2;
			std::vector<MyFileFormat::VertexData> temp;
			for (int i = 0; i < meshes[0].nrOfVertices / 3; i++)
			{
				temp.push_back(vertices[o]);
				temp.push_back(vertices[o - 1]);
				temp.push_back(vertices[o - 2]);
				o += 3;
			}

			if (!CreateVertIndiBuffers(temp, indices, nrOfIndices))
				return false;
		}
		else if (nrOfMeshes > 1)
		{
			for (int i = 0; i < nrOfMeshes; i++)
			{
				std::vector<MyFileFormat::VertexData> vertices;
				std::vector<UINT> indices;
				UINT nrOfIndices = 0;

				this->meshes[i] = AssetLoader::GetModel(i);
				this->vertexSet.insert({i, AssetLoader::GetVertexSet(i)});

				this->nrOfVertices.push_back((UINT)this->meshes[i].nrOfVertices);
				indices.resize(meshes[i].nrOfVertices);
				nrOfIndices = indices.size();
				this->indexCount.push_back(nrOfIndices);
				vertices.resize(nrOfVertices[i]);
				for (int j = 0; j < meshes[i].nrOfVertices; j++)
				{
					//Positions
					vertices[j].positions.X = vertexSet[i][j].positions.X;
					vertices[j].positions.Y = vertexSet[i][j].positions.Y;
					vertices[j].positions.Z = -vertexSet[i][j].positions.Z; //Inverts the Z:axis to be left-handed

					//Normals
					vertices[j].normals.X = vertexSet[i][j].normals.X;
					vertices[j].normals.Y = vertexSet[i][j].normals.Y;
					vertices[j].normals.Z = -vertexSet[i][j].normals.Z; //Inverts the Z:axis to be left-handed

					//UVs
					vertices[j].uvs.X = vertexSet[i][j].uvs.X;
					vertices[j].uvs.Y = -vertexSet[i][j].uvs.Y; //Inverts the V:value to comply to DirectX

					//Tangents
					vertices[j].tangent.X = vertexSet[i][j].tangent.X;
					vertices[j].tangent.Y = vertexSet[i][j].tangent.Y;
					vertices[j].tangent.Z = vertexSet[i][j].tangent.Z;

					//BiTangents
					vertices[j].biTangent.X = vertexSet[i][j].biTangent.X;
					vertices[j].biTangent.Y = vertexSet[i][j].biTangent.Y;
					vertices[j].biTangent.Z = vertexSet[i][j].biTangent.Z;
				}

				//Reverse the draw order of triangle
				size_t o = 2;
				std::vector<MyFileFormat::VertexData> temp;
				for (int i = 0; i < meshes[0].nrOfVertices / 3; i++)
				{
					temp.push_back(vertices[o]);
					temp.push_back(vertices[o - 1]);
					temp.push_back(vertices[o - 2]);
					o += 3;
				}

				if (!CreateVertIndiBuffers(temp, indices, nrOfIndices))
					return false;
			}
		}

		return true;
	}
	else
		return false;
}

void Mesh::Render()
{
	UINT stride = sizeof(MyFileFormat::VertexData);
	UINT offset = 0;
	for (int i = 0; i < this->vertexBuffers.size(); i++)
	{
		Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffers[i], &stride, &offset);
		Graphics::GetContext()->Draw(this->nrOfVertices[i], 0);
		//Graphics::GetContext()->IASetIndexBuffer(this->indexBuffers[i], DXGI_FORMAT_R32_UINT, 0);
		//Graphics::GetContext()->DrawIndexed(this->indexCount[i], 0, 0);
	}
}