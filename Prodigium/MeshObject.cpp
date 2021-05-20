#include "MeshObject.h"
#include "Graphics.h"
#include "ResourceManager.h"

using namespace DirectX::SimpleMath;

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

bool MeshObject::LoadTextures(std::string& diffuse, std::string& normal)
{
	HRESULT hr;

	//Load in the diffuse texture
	if (diffuse != "")
	{
		//To avoid writing long paths to textures
		diffuse = "Textures/" + diffuse;

		ID3D11Texture2D* diffTexture = ResourceManager::GetTexture(diffuse);
		if (diffTexture == nullptr)
		{
			std::cout << "Failed to get a texture from resourceManager..." << std::endl;
			return false;
		}
		hr = Graphics::GetDevice()->CreateShaderResourceView(diffTexture, nullptr, &this->shaderResourceViews[0]);
		if (FAILED(hr))
		{
			std::cout << "Failed to create SRV for diffuse texture" << std::endl;
			return false;
		}
	}
	if (normal != "")
	{
		//To avoid writing long paths to textures
		normal = "Textures/" + normal;

		ID3D11Texture2D* normTexture = ResourceManager::GetTexture(normal);
		if (normTexture == nullptr)
		{
			std::cout << "Failed to get a texture from resourceManager..." << std::endl;
			return false;
		}
		hr = Graphics::GetDevice()->CreateShaderResourceView(normTexture, nullptr, &this->shaderResourceViews[1]);
		if (FAILED(hr))
		{
			std::cout << "Failed to create SRV for normal texture" << std::endl;
			return false;
		}

		//Load normal map settings
		if (!this->SetUpNormalMapBuffer())
		{
			std::cout << "Failed to setup 'has normal map' buffer.\n";
			return false;
		}
	}

	return true;
}

void MeshObject::BuildColliders(const DirectX::SimpleMath::Vector3& min, const DirectX::SimpleMath::Vector3& max)
{
	Collider collider;
	DirectX::SimpleMath::Vector3 corners[8];

	collider.boundingBox.Center.x = (max.x + min.x) / 2.f;
	collider.boundingBox.Center.y = (max.y + min.y) / 2.f;
	collider.boundingBox.Center.z = (max.z + min.z) / 2.f;

	collider.boundingBox.Orientation = { 0.f, 1.f, 0.f, 0.f };
	collider.boundingBox.Extents.x = (max.x - min.x) / 2.f;
	collider.boundingBox.Extents.y = (max.y - min.y) / 2.f;
	collider.boundingBox.Extents.z = (max.z - min.z) / 2.f;

	collider.boundingBox.GetCorners(corners);

	// Front plane
	collider.planes[0].normal = Vector3(Vector3(corners[0] - corners[1])).Cross(Vector3(corners[2] - corners[1]));
	collider.planes[0].point = corners[1];

	// Back plane
	collider.planes[1].normal = collider.planes[0].normal * -1.f;
	collider.planes[1].point = corners[6];

	// Right side plane
	collider.planes[2].normal = Vector3(Vector3(corners[0] - corners[4])).Cross(Vector3(corners[7] - corners[4]));
	collider.planes[2].point = corners[4];

	// Left side plane
	collider.planes[3].normal = collider.planes[2].normal * -1.f;
	collider.planes[3].point = corners[5];

	colliders.push_back(collider);
}

bool MeshObject::LoadColliders()
{
	std::vector<std::vector<DirectX::SimpleMath::Vector3>>positions;

	if (this->mesh != nullptr)
	{
		positions = this->mesh->meshPositions;
	}
	else if (this->animatedObj != nullptr)
	{
		positions.push_back(this->animatedObj->meshPositions);
	}

	//Go through each of the meshes positions
	for (size_t m = 0; m < positions.size(); m++)
	{
		//Find the min and max of the total
		DirectX::SimpleMath::Vector3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
		DirectX::SimpleMath::Vector3 max = { FLT_MIN, FLT_MIN, FLT_MIN };

		for (size_t i = 0; i < positions[m].size(); i++)
		{
			min.x = std::min(min.x, positions[m][i].x);
			min.y = std::min(min.y, positions[m][i].y);
			min.z = std::min(min.z, positions[m][i].z);

			max.x = std::max(max.x, positions[m][i].x);
			max.y = std::max(max.y, positions[m][i].y);
			max.z = std::max(max.z, positions[m][i].z);
		}
		BuildColliders(min, max);
	}
	this->colliders.shrink_to_fit();
	this->collidersOriginal = this->colliders;

	#ifdef _DEBUG
	if (!this->CreateColliderBuffers())
	{
		std::cout << "Failed to create collider buffer..." << std::endl;
		return false;
	}
	#endif
	this->UpdateBoundingBoxes();
	this->UpdateBoundingPlanes();

	return true;
}

void MeshObject::SetRenderColliders()
{
	this->modelCollider = this->mesh->modelCollider;
	this->modelColliderOriginal = this->mesh->modelColliderOriginal;
	this->sphereModelCollider = this->mesh->modelColliderSphere;
}

void MeshObject::UpdateBoundingPlanes()
{
	for (size_t i = 0; i < this->colliders.size(); i++)
	{
		this->colliders[i].planes[0].normal = Vector3::TransformNormal(this->colliders[i].planes[0].normal, this->modelMatrix);
		this->colliders[i].planes[0].normal.Normalize();
		this->colliders[i].planes[1].normal = Vector3::TransformNormal(this->colliders[i].planes[1].normal, this->modelMatrix);
		this->colliders[i].planes[1].normal.Normalize();
		this->colliders[i].planes[2].normal = Vector3::TransformNormal(this->colliders[i].planes[2].normal, this->modelMatrix);
		this->colliders[i].planes[2].normal.Normalize();
		this->colliders[i].planes[3].normal = Vector3::TransformNormal(this->colliders[i].planes[3].normal, this->modelMatrix);
		this->colliders[i].planes[3].normal.Normalize();
	}
}

bool MeshObject::SetUpNormalMapBuffer()
{
	HRESULT hr;

	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = sizeof(DirectX::SimpleMath::Vector4);
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	Vector4 isMapped = { 2.0f, 1.0f, 1.0f, 1.0f };
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &isMapped;
	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->hasNormalMapBuffer);

	return !FAILED(hr);
}

MeshObject::MeshObject()
{
	this->mesh = nullptr;
	this->hasNormalMapBuffer = nullptr;

	for (unsigned int i = 0; i < MAXNROFTEXTURES; i++)
	{
		this->shaderResourceViews[i] = nullptr;
	}

	this->isPickUp = false;
	this->isVisible = true;
	this->isAnimated = false;
	this->animatedObj = nullptr;
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
	if (this->hasNormalMapBuffer)
		this->hasNormalMapBuffer->Release();

	if (this->animatedObj)
		delete this->animatedObj;

	this->colliders.clear();
	this->collidersOriginal.clear();
}

bool MeshObject::Initialize(const std::string& meshObject, 
							std::string diffuse, 
							std::string normal, 
							bool hasBounds, 
							bool hasAnimation, 
							const DirectX::SimpleMath::Vector3& pos, 
							const DirectX::SimpleMath::Vector3& rot, 
							const DirectX::SimpleMath::Vector3& scl)
{
	//Some preparation and setting
	this->isAnimated = hasAnimation;
	BuildMatrix(pos, scl, rot);
	CreateModelMatrixBuffer();
	UpdateMatrix();

	//Load in animation
	if (hasAnimation)
	{
		this->animatedObj = new AnimatedObject();
		if (!this->animatedObj->Initialize(meshObject))
		{
			std::cout << "Failed to initialize animated object..." << std::endl;
			return false;
		}
	}
	//Load in a static mesh
	else
	{
		//Get the mesh from the resource manager if it exist or creates a new mesh
		this->mesh = ResourceManager::GetMesh(meshObject);
		if (this->mesh == nullptr)
		{
			std::cout << "Failed to get a mesh from resourceManager..." << std::endl;
			return false;
		}
	}

	if (!LoadTextures(diffuse, normal))
	{
		std::cout << "Failed to create textures..." << std::endl;
		return false;
	}

	//Create colliders
	this->SetRenderColliders();
	this->UpdateRenderBoundingBox();
	if (hasBounds == true)
	{
		LoadColliders();
	}
	else
	{
		RemoveColliders();
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

void MeshObject::Render(bool shadowPass)
{	
	//If not visible then we can ignore it
	if (this->isVisible)
	{
		//Not necesary to do everything for shadows
		if (!shadowPass)
		{
			//Set all the textures to the geometry pass pixelshader
			for (unsigned int i = 0; i < MAXNROFTEXTURES; i++)
			{
				Graphics::GetContext()->PSSetShaderResources(i, 1, &this->shaderResourceViews[i]);
			}
			Graphics::GetContext()->VSSetConstantBuffers(2, 1, &this->hasNormalMapBuffer);
		}

		if (this->mesh != nullptr)
		{
			Graphics::GetContext()->VSSetConstantBuffers(1, 1, &GetModelMatrixBuffer());
			this->mesh->Render();
		}
		else if (this->animatedObj != nullptr)
		{
			//Standard stuff that static objects use
			ID3D11VertexShader* standardVS;
			Graphics::GetContext()->VSGetShader(&standardVS, nullptr, 0);
			ID3D11InputLayout* standardInputLayout;
			Graphics::GetContext()->IAGetInputLayout(&standardInputLayout);

			if (shadowPass)
				this->animatedObj->Render(GetTransposedMatrix(), false);
			else
				this->animatedObj->Render(GetTransposedMatrix());

			//Set back the normal stuff that static objects use
			Graphics::GetContext()->VSSetShader(standardVS, nullptr, 0);
			Graphics::GetContext()->IASetInputLayout(standardInputLayout);
			standardVS->Release();
			standardInputLayout->Release();
		}
		
	}
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

void MeshObject::UpdateRenderBoundingBox()
{
	this->modelColliderOriginal.boundingBox.Transform(this->modelCollider.boundingBox, this->modelMatrix);
	this->sphereModelCollider.boundingSphere.Transform(this->sphereModelCollider.boundingSphere, this->modelMatrix);
}

void MeshObject::UpdateBoundingBoxes(const Matrix& transform)
{
#ifdef _DEBUG
	D3D11_MAPPED_SUBRESOURCE subResource = {};

	std::vector<DirectX::XMFLOAT3> allCorners;
	DirectX::XMFLOAT3 corners[8];
#endif
	this->modelColliderOriginal.boundingBox.Transform(this->modelCollider.boundingBox, transform);
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

void MeshObject::ChangeAnimState(AnimationState state)
{
	if (this->animatedObj != nullptr)
	{
		this->animatedObj->ChangeAnimState(state);
	}
}
