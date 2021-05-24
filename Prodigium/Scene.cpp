#include "Scene.h"
using namespace DirectX::SimpleMath;

const bool Scene::SetupLightBuffer()
{
	// Remove any traces of old light buffer information.
	if (this->lightBuffer)
	{
		this->lightBuffer->Release();
		this->lightBuffer = nullptr;
	}
	if (this->lightShaderView)
	{
		this->lightShaderView->Release();
		this->lightShaderView = nullptr;
	}

	HRESULT hr, shr;

	// Description for the buffer containing all the light information.
	D3D11_BUFFER_DESC desc = {};
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(LightStruct) * (UINT)this->lights.size();
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.StructureByteStride = sizeof(LightStruct);
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &(this->lights)[0];
	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->lightBuffer);

	if (!(FAILED(hr)))
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.Flags = 0;
		srvDesc.BufferEx.NumElements = (UINT)this->lights.size();

		shr = Graphics::GetDevice()->CreateShaderResourceView(this->lightBuffer, &srvDesc, &this->lightShaderView);

		return !FAILED(shr);
	}

	return false;
}

const bool Scene::SetupInfoBuffer()
{
	HRESULT hr;
	// Description for the info buffer containing amount of lights.
	D3D11_BUFFER_DESC infoDesc = {};
	infoDesc.Usage = D3D11_USAGE_DYNAMIC;
	infoDesc.ByteWidth = sizeof(InfoStruct);
	infoDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	infoDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	infoDesc.MiscFlags = 0;

	hr = Graphics::GetDevice()->CreateBuffer(&infoDesc, NULL, &this->infoBuffer);
	return !FAILED(hr);
}

const bool Scene::UpdateInfoBuffer()
{
	InfoStruct newInfo = {};
	newInfo.info = DirectX::XMFLOAT4((float)lights.size(), (float)this->shadowHandler->NrOfShadows(), 0.0f, 0.0f);

	D3D11_MAPPED_SUBRESOURCE submap;
	HRESULT hr;
	hr = Graphics::GetContext()->Map(this->infoBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &submap);
	memcpy(submap.pData, &newInfo, sizeof(DirectX::XMFLOAT4));
	Graphics::GetContext()->Unmap(this->infoBuffer, 0);
	return !FAILED(hr);
}

Scene::Scene()
{
	this->currentObject = 0;
	this->currentLight = 0;
	this->lightBuffer = nullptr;
	this->lightShaderView = nullptr;
	this->infoBuffer = nullptr;
	this->firstTime = true;
	this->menuMode = false;
	this->shadowHandler = new ShadowHandler();

	LightStruct filler;
	filler.position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	filler.direction = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	filler.attentuate = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, -1.0f);
	lights.push_back(filler);
}

Scene::~Scene()
{
	if (this->lightShaderView)
		this->lightShaderView->Release();
	if (this->lightBuffer)
		this->lightBuffer->Release();
	if (this->infoBuffer)
		this->infoBuffer->Release();

	if (this->shadowHandler)
		delete this->shadowHandler;
	// Delete the allocated memory from vector.
	for (int i = 0; i < (int)objects.size(); i++)
	{
		delete this->objects[i];
	}

}

void Scene::Add(const std::string& objFile, 
				const std::string& diffuseTxt, 
				const std::string& normalTxt, 
				bool hasBounds, 
				bool hasAnimation, 
				const Vector3& position,
				const Vector3& rotation, 
				const Vector3& scale)
{
	/*
		Create a new MeshObject from input.
	*/
	MeshObject* newObject = new MeshObject;
	if (newObject->Initialize(objFile, diffuseTxt, normalTxt, hasBounds, hasAnimation, position, rotation, scale))
	{
		this->objects.push_back(newObject);
		this->currentObject = (int)objects.size() - 1;
	}
	else
	{
#ifdef _DEBUG
		std::cout << "Failed to add object" << std::endl;
#endif
	}
}

void Scene::AddLight(LightStruct& L)
{
	this->lights.push_back(L);
	this->firstTime = true;
	this->shadowHandler->Add(L);
}

void Scene::PopLight()
{
	if ((int)this->lights.size() > 1)
	{
		this->lights.pop_back();
		this->firstTime = true;
	}
}

void Scene::PopAllLights()
{
#ifdef _DEBUG
	std::cout << "Number of Lights removed: " << (int)this->lights.size() - 1 << "\n";
#endif
	while ((int)this->lights.size() > 1)
	{
		this->lights.pop_back();
		this->firstTime = true;
	}
}

void Scene::Add(MeshObject* object)
{
	if (object != nullptr)
	{
		this->objects.push_back(object);
		this->currentObject = (int)objects.size() - 1;
	}
}

void Scene::UpdateMatrix(const Vector3& pos, const Vector3& rotation, const Vector3& scale)
{
	/*
		Update the Matrix with input.
	*/
	if (!this->objects[this->currentObject]->UpdateMatrix(pos, scale, rotation))
	{
#ifdef _DEBUG
		std::cout << "Error with updating the Matrix of object: " << this->currentObject << "\n";
#endif
	}

#ifdef _DEBUG
	//std::cout << "Matrix was updated for object on index " << this->currentObject << ". \n";
#endif

}

void Scene::RemoveObject(const int& index)
{
	if (index < (int)this->objects.size() - 1 && index >= 0)
	{
		if (this->objects[index] != nullptr)
		{
			this->currentObject = 0;
			delete this->objects[index];
			this->objects.erase(this->objects.begin() + index);
		}
		else
		{
#ifdef _DEBUG
			std::cout << "Object was a nullptr!\n";
#endif
		}
	}
	else
	{
#ifdef _DEBUG

		std::cout << "index is outside of vector scope!\n";
		std::cout << "index was: " << index << ". Scope is: " << 0 << " to " << (int)this->objects.size() - 1 << "\n";
#endif
	}
}

void Scene::SwitchObject(const int& index)
{
	if (index < (int)this->objects.size() && index >= 0)
		this->currentObject = index;
	else
	{
#ifdef _DEBUG
		std::cout << "index is outside of vector scope!\n";
#endif
	}
}


MeshObject& Scene::GetMeshObject(int index)
{
	return *this->objects[index];
}

const std::vector<MeshObject*>& Scene::GetAllMeshObjects()
{
	return this->objects;
}

ShadowHandler& Scene::GetShadows()
{
	return *this->shadowHandler;
}

ParticleSystem& Scene::GetParticles()
{
	return this->particles;
}

const int Scene::GetNumberOfObjects() const
{
	return (unsigned int)this->objects.size();
}

void Scene::RemoveAllObjects()
{
#ifdef _DEBUG
	std::cout << "Amount of objects deleted: " << (int)objects.size() << "\n";
#endif
	while ((int)objects.size() > 0)
	{
		delete this->objects[(int)this->objects.size() - 1];
		this->objects.pop_back();
	}
	this->Pop();
}

void Scene::Pop()
{
	if ((int)this->objects.size() > 0)
	{
		delete this->objects[(unsigned int)this->objects.size() - 1];
		this->objects.pop_back();
	}
}

void Scene::Render()
{
	if ((int)this->objects.size() > 0)
	{
		for (int i = 0; i < (int)this->objects.size(); i++)
		{
			this->objects[i]->Render();
		}
	}
}

void Scene::Render(const std::unordered_map<std::uintptr_t, MeshObject*>& toRender)
{
	//if ((int)toRender.size() > 0)
	//{
	//	for (int i = 0; i < (int)toRender.size(); i++)
	//	{
	//		if (toRender[i]->IsVisible())
	//		{
	//			toRender[i]->Render();
	//		}
	//	}
	//}
	for (auto object : toRender)
	{
		if (object.second->IsVisible())
		{
			object.second->Render();
		}
	}
#ifdef _DEBUG
	std::cout << "Active: " << toRender.size() << " Total: " << this->objects.size() << "\r";
#endif
}

void Scene::RenderLights()
{
	if ((int)this->lights.size() > 0)
	{
		if (this->firstTime)
		{
			this->firstTime = false;
			if (!this->SetupLightBuffer())
			{
#ifdef _DEBUG
				std::cout << "Error making light" << "\n";
#endif
				return;
			}
			else
			{
				/*
				* If new light setup did not fail we update the info buffer also
				* sent to GPU.
				*/
				this->UpdateInfoBuffer();
			}
		}

		Graphics::GetContext()->PSSetConstantBuffers(0, 1, &this->infoBuffer);
		Graphics::GetContext()->PSSetShaderResources(3, 1, &this->lightShaderView);
	}
}

void Scene::RenderShadows()
{
	this->shadowHandler->Prepare();

	for (int i = 0; i < shadowHandler->NrOfShadows(); i++)
	{
		this->shadowHandler->Render(i);

		// Loop through all objects
		for (int j = 0; j < (int)objects.size(); j++)
		{
			// Check the distance between light source and object.
			if (this->objects[j]->GetDistance(this->shadowHandler->GetShadow(i).GetPos()) < SHADOWRANGE && this->objects[j]->IsVisible())
			{
				this->objects[j]->Render(true);
			}
		}
	}
	this->shadowHandler->Clear();
}

void Scene::RenderShadows(const std::unordered_map<std::uintptr_t, MeshObject*>& toRender)
{
	this->shadowHandler->Prepare();
	for (int i = 0; i < shadowHandler->NrOfShadows(); i++)
	{
		this->shadowHandler->Render(i);

		// Loop through all objects
		for (auto object : toRender)
		{
			if (object.second->GetDistance(this->shadowHandler->GetShadow(i).GetPos()) < SHADOWRANGE && object.second->IsVisible())
			{
				object.second->Render(true);
			}
		}
	}
	this->shadowHandler->Clear();
}

void Scene::RenderParticles()
{
	if (this->particles.IsActive())
	{
		this->particles.Render();
		this->particles.UpdateSpeedBuffer(this->objects[0]->GetPosition(), this->objects[6]->GetPosition());
	}

}

void Scene::SwitchMenuMode(bool sw)
{
	this->menuMode = sw;
}

void Scene::ClearCullingObjects()
{
	this->visibleObjects.clear();
}

std::unordered_map<std::uintptr_t, MeshObject*>& Scene::GetAllCullingObjects()
{
	return this->visibleObjects;
}

#ifdef _DEBUG
void Scene::RenderBoundingBoxes(const std::unordered_map<std::uintptr_t, MeshObject*>& toRender)
{
	if ((int)toRender.size() > 0)
	{
		for (auto object : toRender)
		{
			object.second->RenderBoundingBoxes();
		}
		//for (int i = 0; i < (int)toRender.size(); i++)
		//{
		//	toRender[i]->RenderBoundingBoxes();
		//}
	}
}
#endif
