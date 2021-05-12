#include "MeshObject.h"
#include "Graphics.h"
using namespace DirectX::SimpleMath;
bool MeshObject::BindTextureToSRV(ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv)
{
	HRESULT hr = Graphics::GetDevice()->CreateShaderResourceView(texture, nullptr, &srv);
	return !FAILED(hr);
}

#ifdef _DEBUG
bool MeshObject::CreateColliderBuffers()
{
	// Gets the corners of the boundingBox and places it in it's vertexbuffer

	HRESULT hr;

	std::vector<DirectX::XMFLOAT3> allCorners;
	DirectX::XMFLOAT3 corners[8];

	for (int i = 0; i < this->colliders.size(); i++)
	{
		this->colliders[i].boundingBox.GetCorners(corners);

		allCorners.push_back(corners[1]);
		allCorners.push_back(corners[0]);
		allCorners.push_back(corners[3]);
		allCorners.push_back(corners[2]);
		allCorners.push_back(corners[1]);
		allCorners.push_back(corners[5]);
		allCorners.push_back(corners[6]);
		allCorners.push_back(corners[2]);
		allCorners.push_back(corners[3]);
		allCorners.push_back(corners[7]);
		allCorners.push_back(corners[6]);
		allCorners.push_back(corners[7]);
		allCorners.push_back(corners[4]);
		allCorners.push_back(corners[0]);
		allCorners.push_back(corners[4]);
		allCorners.push_back(corners[5]);

		D3D11_BUFFER_DESC desc = {};

		desc.ByteWidth = (UINT)allCorners.size() * sizeof(DirectX::XMFLOAT3);
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

		D3D11_SUBRESOURCE_DATA data = {};

		data.pSysMem = allCorners.data();

		ID3D11Buffer* boxBuffer = nullptr;

		hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &boxBuffer);

		boundingBoxBuffer.push_back(boxBuffer);

		if (FAILED(hr))
		{
			return false;
		}

		allCorners.clear();
	}

	return true;
}
#endif

void MeshObject::SetColliders()
{
	this->colliders = this->mesh->colliders;
	this->collidersOriginal = this->mesh->collidersOriginal;

	DirectX::XMFLOAT3 corners[8];

	for (int i = 0; i < this->colliders.size(); i++)
	{
		this->colliders[i].boundingBox.GetCorners(corners);

		// Front plane
		this->colliders[i].planes[0].normal = Vector3(Vector3(corners[0] - corners[1])).Cross(Vector3(corners[2] - corners[1]));
		this->colliders[i].planes[0].point = corners[1];
		this->colliders[i].planes[0].normal.Normalize();

		// Back plane
		this->colliders[i].planes[1].normal = this->colliders[i].planes[0].normal * -1.f;
		this->colliders[i].planes[1].point = corners[6];
		this->colliders[i].planes[1].normal.Normalize();

		// Right side plane
		this->colliders[i].planes[2].normal = Vector3(Vector3(corners[0] - corners[4])).Cross(Vector3(corners[7] - corners[4]));
		this->colliders[i].planes[2].point = corners[4];
		this->colliders[i].planes[2].normal.Normalize();

		// Left side plane
		this->colliders[i].planes[3].normal = this->colliders[i].planes[2].normal * -1.f;
		this->colliders[i].planes[3].point = corners[5];
		this->colliders[i].planes[3].normal.Normalize();

		//// Top side plane
		//this->colliders[i].planes[4].normal = Vector3(Vector3(corners[7] - corners[6])).Cross(Vector3(corners[2] - corners[6]));
		//this->colliders[i].planes[4].point = corners[7];
		//this->colliders[i].planes[4].normal.Normalize();

		//// Bottom side plane
		//this->colliders[i].planes[5].normal = this->colliders[i].planes[4].normal * -1.f;
		//this->colliders[i].planes[5].point = corners[4];
		//this->colliders[i].planes[5].normal.Normalize();

		//std::cout << "Front: " << this->colliders[i].planes[0].normal.x << " " << this->colliders[i].planes[0].normal.y << " " << this->colliders[i].planes[0].normal.z << std::endl;
		//std::cout << "Back: " << this->colliders[i].planes[1].normal.x << " " << this->colliders[i].planes[1].normal.y << " " << this->colliders[i].planes[1].normal.z << std::endl;
		//std::cout << "Right: " << this->colliders[i].planes[2].normal.x << " " << this->colliders[i].planes[2].normal.y << " " << this->colliders[i].planes[2].normal.z << std::endl;
		//std::cout << "Left: " << this->colliders[i].planes[3].normal.x << " " << this->colliders[i].planes[3].normal.y << " " << this->colliders[i].planes[3].normal.z << std::endl;
		//std::cout << "Top: " << this->colliders[i].planes[4].normal.x << " " << this->colliders[i].planes[4].normal.y << " " << this->colliders[i].planes[4].normal.z << std::endl;
		//std::cout << "Bottom: " << this->colliders[i].planes[5].normal.x << " " << this->colliders[i].planes[5].normal.y << " " << this->colliders[i].planes[5].normal.z << std::endl;
	}
}

MeshObject::MeshObject()
{
	this->mesh = nullptr;

	for (unsigned int i = 0; i < MAXNROFTEXTURES; i++)
	{
		this->shaderResourceViews[i] = nullptr;
	}

	this->isPickUp = false;
	this->isVisible = true;
}

MeshObject::~MeshObject()
{
	for (unsigned int i = 0; i < MAXNROFTEXTURES; i++)
	{
		if (this->shaderResourceViews[i])
			this->shaderResourceViews[i]->Release();
	}
#ifdef _DEBUG
	for (size_t i = 0; i < this->boundingBoxBuffer.size(); i++)
	{
		if (this->boundingBoxBuffer[i])
			this->boundingBoxBuffer[i]->Release();
	}
	this->boundingBoxBuffer.clear();
#endif

	this->colliders.clear();
	this->collidersOriginal.clear();
}

bool MeshObject::Initialize(std::string meshObject, std::string diffuseTxt, std::string normalTxt, bool hasBounds, Vector3 pos, Vector3 rot, Vector3 scl)
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

	if (hasBounds == true)
	{
		this->SetColliders();

#ifdef _DEBUG
		this->CreateColliderBuffers();
#endif
		this->UpdateBoundingBoxes();

	}
	else
	{
		this->RemoveColliders();
	}

	this->CreateModelMatrixBuffer();

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

void MeshObject::UpdateBoundingBoxes()
{
#ifdef _DEBUG
	D3D11_MAPPED_SUBRESOURCE subResource = {};

	std::vector<DirectX::XMFLOAT3> allCorners;
	DirectX::XMFLOAT3 corners[8];
#endif

	for (size_t i = 0; i < this->colliders.size(); i++)
	{
		this->collidersOriginal[i].boundingBox.Transform(this->colliders[i].boundingBox, this->modelMatrix);

#ifdef _DEBUG
		colliders[i].boundingBox.GetCorners(corners);

		allCorners.push_back(corners[1]);
		allCorners.push_back(corners[0]);
		allCorners.push_back(corners[3]);
		allCorners.push_back(corners[2]);
		allCorners.push_back(corners[1]);
		allCorners.push_back(corners[5]);
		allCorners.push_back(corners[6]);
		allCorners.push_back(corners[2]);
		allCorners.push_back(corners[3]);
		allCorners.push_back(corners[7]);
		allCorners.push_back(corners[6]);
		allCorners.push_back(corners[7]);
		allCorners.push_back(corners[4]);
		allCorners.push_back(corners[0]);
		allCorners.push_back(corners[4]);
		allCorners.push_back(corners[5]);
		allCorners.push_back(corners[4]);
		allCorners.push_back(corners[5]);
		allCorners.push_back(corners[6]);
		allCorners.push_back(corners[7]);

		Graphics::GetContext()->Map(this->boundingBoxBuffer[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy(subResource.pData, allCorners.data(), allCorners.size() * sizeof(DirectX::XMFLOAT3));
		Graphics::GetContext()->Unmap(this->boundingBoxBuffer[i], 0);

		allCorners.clear();
#endif
	}
}

void MeshObject::UpdateBoundingBoxes(const Matrix& transform)
{
#ifdef _DEBUG
	D3D11_MAPPED_SUBRESOURCE subResource = {};

	std::vector<DirectX::XMFLOAT3> allCorners;
	DirectX::XMFLOAT3 corners[8];
#endif

	for (size_t i = 0; i < this->colliders.size(); i++)
	{
		this->collidersOriginal[i].boundingBox.Transform(this->colliders[i].boundingBox, transform);

#ifdef _DEBUG
		colliders[i].boundingBox.GetCorners(corners);

		allCorners.push_back(corners[1]);
		allCorners.push_back(corners[0]);
		allCorners.push_back(corners[3]);
		allCorners.push_back(corners[2]);
		allCorners.push_back(corners[1]);
		allCorners.push_back(corners[5]);
		allCorners.push_back(corners[6]);
		allCorners.push_back(corners[2]);
		allCorners.push_back(corners[3]);
		allCorners.push_back(corners[7]);
		allCorners.push_back(corners[6]);
		allCorners.push_back(corners[7]);
		allCorners.push_back(corners[4]);
		allCorners.push_back(corners[0]);
		allCorners.push_back(corners[4]);
		allCorners.push_back(corners[5]);

		Graphics::GetContext()->Map(this->boundingBoxBuffer[i], 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		memcpy(subResource.pData, allCorners.data(), allCorners.size() * sizeof(DirectX::XMFLOAT3));
		Graphics::GetContext()->Unmap(this->boundingBoxBuffer[i], 0);

		allCorners.clear();
#endif
	}
}

const bool MeshObject::IsVisible() const
{
	return this->isVisible;
}

#ifdef _DEBUG
void MeshObject::RenderBoundingBoxes()
{
	UINT stride = sizeof(DirectX::XMFLOAT3);
	UINT offset = 0;
	UINT nrOfCorners = 16;

	for (int i = 0; i < this->boundingBoxBuffer.size(); i++)
	{
		Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->boundingBoxBuffer[i], &stride, &offset);
		Graphics::GetContext()->Draw(nrOfCorners, 0);
	}
}
#endif

void MeshObject::RemoveColliders()
{
	this->colliders.clear();
	this->collidersOriginal.clear();
}