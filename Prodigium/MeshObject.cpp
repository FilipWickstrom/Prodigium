#include "MeshObject.h"

MeshObject::MeshObject()
{
    this->vertexCount = 0;
    this->vertexBuffer = 0;
    this->diffuseMap = 0;
    this->normalMap = 0;
    this->normalMapResourceView = 0;
    this->diffuseMapResourceView = 0;

    this->isPickUp = false;
    this->isVisible = false;
}

MeshObject::~MeshObject()
{
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
    /*
        Set vertexBuffer as IASET...vertexbuffer,
        Anything more needed before rendering then add.
    */

    context->Draw(this->vertexCount, 0);
}
