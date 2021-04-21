#include "MeshObject.h"

MeshObject::MeshObject()
{


    this->isPickUp = false;
    this->isVisible = false;
}

MeshObject::~MeshObject()
{
}

bool MeshObject::Initialize(std::string meshObject)
{
    if (!this->mesh.LoadFile(meshObject))
    {
        return false;
    }

    return true;
}

void MeshObject::SetVisible(bool toggle)
{
    this->isVisible = toggle;
}

void MeshObject::SetPickUp(bool toggle)
{
    this->isPickUp = toggle;
}

void MeshObject::Render()
{
    //Set this objects modelmatrix
    Graphics::GetContext()->VSSetConstantBuffers(1, 1, &GetModelMatrixBuffer());

    this->mesh.Render();
}
