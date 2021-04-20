#include "MeshObject.h"

MeshObject::MeshObject()
    :GameObject()
{
    this->vertexCount = 0;
    this->indexCount = 0;
    this->vertexBuffer = nullptr;
    this->indexBuffer = nullptr;
    this->diffuseMap = nullptr;
    this->normalMap = nullptr;
    this->normalMapResourceView = nullptr;
    this->diffuseMapResourceView = nullptr;

    this->isPickUp = false;
    this->isVisible = false;

    //TEMP*****
    viewProjmatrix = {};
    viewProjBuffer = nullptr;
}

MeshObject::~MeshObject()
{
    if (this->vertexBuffer)
        this->vertexBuffer->Release();
    if (this->indexBuffer)
        this->indexBuffer->Release();
    if (this->diffuseMap)
        this->diffuseMap->Release();
    if (this->normalMap)
        this->normalMap->Release();
    if (this->normalMapResourceView)
        this->normalMapResourceView->Release();
    if (this->diffuseMapResourceView)
        this->diffuseMapResourceView->Release();
}

bool MeshObject::CreateVertIndiBuffers(ID3D11Device* device, std::vector<Vertex>& vertices, std::vector<unsigned short>& indices)
{
    /*-----Vertexbuffer-----*/
    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = sizeof(Vertex) * this->vertexCount;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;

    D3D11_SUBRESOURCE_DATA data = {};
    data.pSysMem = &vertices[0];
    data.SysMemPitch = 0;
    data.SysMemSlicePitch = 0;

    HRESULT hr = device->CreateBuffer(&desc, &data, &this->vertexBuffer);
    if (FAILED(hr))
    {
        std::cout << "Failed to create vertex buffer..." << std::endl;
        return false;
    }

    /*-----Indexbuffer-----*/
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.ByteWidth = sizeof(unsigned short) * this->indexCount;
    data.pSysMem = &indices[0];

    hr = device->CreateBuffer(&desc, &data, &this->indexBuffer);
    if (FAILED(hr))
    {
        std::cout << "Failed to create index buffer..." << std::endl;
        return false;
    }

    return true;
}

//CLEAN UP THE INITIALIZE! Should later have - build matrix inside of it
bool MeshObject::Initialize(ID3D11Device* device, std::string filename)
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
        /*FIX A LOOP SO THAT WE CAN LOAD IN A MODEL WITH MORE THAN ONE MESH IN IT*/
        /*
        for (unsigned int i = 0; i < scene->mNumMeshes)
        {
            const aiMesh* mesh = scene->mMeshes[i];
            
            LoadMesh(mesh,)
                l--->LoadMaterial()
                l--->LoadTextures()
        }
        */

        //Only works with one mesh at this time
        if (scene->mNumMeshes == 1)
        {
            const aiMesh* mesh = scene->mMeshes[0];

            std::vector<Vertex> vertices;
            this->vertexCount = mesh->mNumVertices;
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
            if (!CreateVertIndiBuffers(device, vertices, indices))
            {
                std::cout << "Failed to create vertex and index buffers..." << std::endl;
                return false;
            }

            
            
            if (scene->HasMaterials())
            {
                const aiMaterial* material = scene->mMaterials[0];
                unsigned int nrOfDiffuse = material->GetTextureCount(aiTextureType_DIFFUSE);
                std::cout << "Diffuse textures: " << nrOfDiffuse << std::endl;

                aiString diffusePath;
                if (material->GetTexture(aiTextureType_DIFFUSE, 0, &diffusePath) == AI_SUCCESS)
                {
                    std::cout << "Got the texture: " << diffusePath.C_Str() << std::endl;
                }

            }
            else
            {
                std::cout << "Scene does not have any materials..." << std::endl;
            }

            //***For debugging for now***
            std::cout << "Model: " << filename << " was successfully loaded! Vertices: " << this->vertexCount << std::endl;

            //LOAD IN TEXTURES LATER
            //LOAD IN MATERIALS
            //LATER USE: Load in animations? mesh->hasBones()
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

    //-------------------TEMP START--------------------
    DirectX::XMFLOAT4X4 view;
    DirectX::XMFLOAT4X4 proj;
    XMStoreFloat4x4(&proj, DirectX::XMMatrixPerspectiveFovLH(0.4f * DirectX::XM_PI, float(16.0f / 9.0f), 0.1f, 100.0f));
    DirectX::XMVECTOR eye = { 0.0f, 0.0f, 0.0f, 0.0f };
    DirectX::XMVECTOR target = { 0.0f, 0.0f, 1.0f, 0.0f };
    DirectX::XMVECTOR up = { 0.0f, 1.0f, 0.0f, 0.0f };
    XMStoreFloat4x4(&view, DirectX::XMMatrixLookAtLH(eye, target, up));

    viewProjmatrix.projection = proj;
    viewProjmatrix.view = view;

    D3D11_BUFFER_DESC desc;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.ByteWidth = sizeof(this->viewProjmatrix);
    D3D11_SUBRESOURCE_DATA data;
    data.pSysMem = &this->viewProjmatrix;
    HRESULT result = device->CreateBuffer(&desc, &data, &this->viewProjBuffer);
    return !FAILED(result);
    //-------------------TEMP END--------------------

    //return true;
}

void MeshObject::SetVisible(bool toggle)
{
    this->isVisible = toggle;
}

void MeshObject::SetPickUp(bool toggle)
{
    this->isPickUp = toggle;
}

bool MeshObject::LoadMesh(ID3D11Device* device, std::string filePath)
{
    /*
        Here to update vertexCount and VertexBuffer.
    */
    return false;
}

bool MeshObject::LoadDiffuseTexture(ID3D11Device* device, std::string filePath)
{
    /*
        Update diffuseMap and Shader View associate with it.
    */
    return false;
}

bool MeshObject::LoadNormalTexture(ID3D11Device* device, std::string filePath)
{
    /*
       Update normalMap and Shader View associate with it.
    */
    return false;
}

void MeshObject::Render(ID3D11DeviceContext*& context)
{

    context->VSSetConstantBuffers(0, 1, &this->viewProjBuffer);     //TEMP. CAMERA SHOULD DO THIS LATER!*****
   
    //Set the modelmatrix
    context->VSSetConstantBuffers(1, 1, &GetModelMatrixBuffer());

    /*
    for every mesh
        set modelmatrix for that mesh


    */

    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    context->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
    context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    context->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R16_UINT, 0);
    context->DrawIndexed(this->indexCount, 0, 0);

}
