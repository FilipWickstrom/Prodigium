#include "Mesh.h"

bool Mesh::CreateVertIndiBuffers(std::vector<Vertex>& vertices, std::vector<unsigned short>& indices)
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

    HRESULT hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->vertexBuffer);
    if (FAILED(hr))
    {
        std::cout << "Failed to create vertex buffer..." << std::endl;
        return false;
    }

    /*-----Indexbuffer-----*/
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.ByteWidth = sizeof(unsigned short) * this->indexCount;
    data.pSysMem = &indices[0];

    hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->indexBuffer);
    if (FAILED(hr))
    {
        std::cout << "Failed to create index buffer..." << std::endl;
        return false;
    }

    return true;
}

Mesh::Mesh()
	:Resource(ResourceType::MESH)
{
	this->indexCount = 0;
	this->objectFile = "";
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
}

Mesh::~Mesh()
{
	if (this->vertexBuffer)
		this->vertexBuffer = nullptr;
	if (this->indexBuffer)
		this->indexBuffer = nullptr;
}

bool Mesh::LoadFile(std::string filename)
{
    //JUST SOME TESTING
    Assimp::Importer importer;

    //Load in the scene - can be many meshes together in one file
    const aiScene* scene = importer.ReadFile("Models/" + filename,
                                            aiProcess_Triangulate |               //Triangulate every surface
                                            aiProcess_JoinIdenticalVertices |     //Ignores identical veritices - memory saving
                                            aiProcess_MakeLeftHanded);            //Use a lefthanded system for the models
                                            //aiProcess_CalcTangentSpace);        //LATER FIX: can be added to fix tangents automatic
                                            //aiProcess_FlipUVs                   //If textures gets in the wrong direction...

    //Reading file: Success
    if (scene != nullptr)
    {
        //Only works with one mesh at this time
        if (scene->mNumMeshes == 1)
        {
            const aiMesh* mesh = scene->mMeshes[0];

            std::vector<Vertex> vertices;
            vertices.reserve(mesh->mNumVertices);

            //Loading in all the vertices to the right structure
            for (unsigned int i = 0; i < mesh->mNumVertices; i++)
            {
                Vertex temp;
                temp.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
                temp.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
                temp.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
                vertices.push_back(temp);
            }

            std::vector<unsigned short> indices;
            this->indexCount = mesh->mNumFaces * 3;
            indices.reserve(mesh->mNumFaces * 3);

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
            if (!CreateVertIndiBuffers(vertices, indices))
            {
                std::cout << "Failed to create vertex and index buffers..." << std::endl;
                return false;
            }

            std::cout << "Model: " << filename << " was successfully loaded!" << std::endl;
        }
        else
        {
            /*
            LATER FIX: Should be possible to load in files with more than one mesh in
            */
            std::cout << "Only works with one mesh for now..." << std::endl;
            std::cout << "Nr of meshes is: " << scene->mNumMeshes << std::endl;
            aiReleaseImport(scene);
            return false;
        }
    }
    else
    {
        std::cout << "Could not find the file " << filename << std::endl;
        aiReleaseImport(scene);
        return false;
    }

	return false;
}

void Mesh::Render()
{
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
    Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Graphics::GetContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    Graphics::GetContext()->DrawIndexed(this->indexCount, 0, 0);
}
