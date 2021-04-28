#include "MeshObject.h"
#include "Graphics.h"

bool MeshObject::BindTextureToSRV(ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv)
{
    HRESULT hr = Graphics::GetDevice()->CreateShaderResourceView(texture, nullptr, &srv);
    return !FAILED(hr);
}

MeshObject::MeshObject()
{
    this->mesh = nullptr;

    for (unsigned int i = 0; i < MAXNROFTEXTURES; i++)
    {
        this->shaderResourceViews[i] = nullptr;
    }

    this->isPickUp = false;
    this->isVisible = false;
}

MeshObject::~MeshObject()
{
    for (unsigned int i = 0; i < MAXNROFTEXTURES; i++)
    {
        if (this->shaderResourceViews[i])
            this->shaderResourceViews[i]->Release();
    }
}

bool MeshObject::Initialize(std::string meshObject, std::string diffuseTxt, std::string normalTxt, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 scl)
{
    //Get the mesh from the resource manager if it exist or creates a new mesh
    this->mesh = ResourceManager::GetMesh(meshObject);
    if (this->mesh == nullptr)
    {
        std::cout << "Failed to get a mesh from resourceManager..." << std::endl;
        return false;
    }

    //Load in the diffuse texture
    if (diffuseTxt != "")
    {
        //To avoid writing long paths to textures
        diffuseTxt = "Textures/" + diffuseTxt;

        ID3D11Texture2D* diffTexture = ResourceManager::GetTexture(diffuseTxt);
        if (diffTexture == nullptr)
        {
            std::cout << "Failed to get a texture from resourceManager..." << std::endl;
            return false;
        }
        if (!BindTextureToSRV(diffTexture, this->shaderResourceViews[0]))
        {
            std::cout << "Failed to bind the texture to the shaderResourceView..." << std::endl;
            return false;
        }
    }

    //Load in the normal map
    if (normalTxt != "")
    {
        ID3D11Texture2D* normTexture = ResourceManager::GetTexture(normalTxt);
        if (normTexture == nullptr)
        {
            std::cout << "Failed to get a texture from resourceManager..." << std::endl;
            return false;
        }
        if (!BindTextureToSRV(normTexture, this->shaderResourceViews[1]))
        {
            std::cout << "Failed to bind the texture to the shaderResourceView..." << std::endl;
            return false;
        }
    }

    if (!BuildMatrix(pos, scl, rot))
    {
        std::cout << "Build matrix failed..." << std::endl;
        return false;
    }

    //EXTRA: Material

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
    
    //Set all the textures to the geometry pass pixelshader
    for (unsigned int i = 0; i < MAXNROFTEXTURES; i++)
    {
        Graphics::GetContext()->PSSetShaderResources(i, 1, &this->shaderResourceViews[i]);
    }

    this->mesh->Render();
}
