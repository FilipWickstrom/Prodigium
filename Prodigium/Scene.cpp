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
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.ByteWidth = sizeof(LightStruct) * (UINT)this->lights.size();
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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
	for (int i = 0; i < (int)this->objects.size(); i++)
	{
		delete this->objects[i];
	}
	this->objects.clear();

	for (int i = 0; i < (int)this->dynamicObjects.size(); i++)
	{
		delete this->dynamicObjects[i];
	}
	this->dynamicObjects.clear();
	this->staticObjects.clear();
	this->visibleObjects.clear();
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

void Scene::UpdateLightsBuffer()
{
	D3D11_MAPPED_SUBRESOURCE resource;
	Graphics::GetContext()->Map(this->lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	memcpy(resource.pData, &this->lights[0], sizeof(LightStruct) * this->lights.size());
	Graphics::GetContext()->Unmap(this->lightBuffer, 0);
}

void Scene::Add(MeshObject* object)
{
	if (object != nullptr)
	{
		this->objects.push_back(object);
		this->currentObject = (int)objects.size() - 1;
	}
}

void Scene::AddDynamicObject(const std::string& objFile, const std::string& diffuseTxt, const std::string& normalTxt, bool hasBounds, bool hasAnimation, const DirectX::SimpleMath::Vector3& position, const DirectX::SimpleMath::Vector3& rotation, const DirectX::SimpleMath::Vector3& scale)
{
	/*
		Create a new MeshObject from input.
	*/
	MeshObject* newObject = new MeshObject;
	if (newObject->Initialize(objFile, diffuseTxt, normalTxt, hasBounds, hasAnimation, position, rotation, scale))
	{
		this->dynamicObjects.push_back(newObject);
	}
	else
	{
#ifdef _DEBUG
		std::cout << "Failed to add object" << std::endl;
#endif
	}
}

void Scene::AddDynamicObject(MeshObject* object)
{
	if (object != nullptr)
	{
		this->dynamicObjects.push_back(object);
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

MeshObject& Scene::GetDynamicObject(int index)
{
	return *this->dynamicObjects[index];
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
	while ((int)dynamicObjects.size() > 0)
	{
		delete this->dynamicObjects[(int)this->dynamicObjects.size() - 1];
		this->dynamicObjects.pop_back();
	}

}

void Scene::Pop()
{
	if ((int)this->objects.size() > 0)
	{
		delete this->objects[(unsigned int)this->objects.size() - 1];
		this->objects.pop_back();
		delete this->dynamicObjects[(unsigned int)this->dynamicObjects.size() - 1];
		this->dynamicObjects.pop_back();
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

void Scene::RenderStaticObjects()
{
	for (auto object : this->staticObjects)
	{
		if (object.second->IsVisible())
		{
			object.second->Render();
		}
	}
#ifdef _DEBUG
	//std::cout << "Active: " << this->staticObjects.size() << " Total: " << this->objects.size() << "\r";
#endif
}

void Scene::RenderDynamicObjects()
{
	if ((int)this->dynamicObjects.size() > 0)
	{
		for (int i = 0; i < (int)this->dynamicObjects.size(); i++)
		{
			if (this->dynamicObjects[i]->IsVisible())
			{
				this->dynamicObjects[i]->Render();
			}
		}
	}
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
				this->objects[j]->RenderShadows();
			}
		}
	}
	this->shadowHandler->Clear();
}

void Scene::RenderStaticShadows()
{
	this->shadowHandler->Prepare();
	for (int i = 0; i < shadowHandler->NrOfShadows(); i++)
	{
		this->shadowHandler->Render(i);

		// Loop through all objects
		for (auto object : this->staticObjects)
		{
			if (object.second->GetDistance(this->shadowHandler->GetShadow(i).GetPos()) < SHADOWRANGE && object.second->IsVisible())
			{
				object.second->RenderShadows();
			}
		}
	}
	this->shadowHandler->Clear();
}

void Scene::RenderDynamicShadows()
{
	this->shadowHandler->Prepare();

	for (int i = 0; i < shadowHandler->NrOfShadows(); i++)
	{
		this->shadowHandler->Render(i);

		// Loop through all objects
		for (int j = 0; j < (int)dynamicObjects.size(); j++)
		{
			// Check the distance between light source and object.
			if (this->dynamicObjects[j]->GetDistance(this->shadowHandler->GetShadow(i).GetPos()) < SHADOWRANGE && this->dynamicObjects[j]->IsVisible())
			{
				this->dynamicObjects[j]->RenderShadows();
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
		this->particles.UpdateSpeedBuffer(this->objects[0]->GetPosition(), this->objects[1]->GetPosition());
	}

}

void Scene::RenderSSAO()
{
	this->SSAOcclusion.Render();
}

void Scene::RenderSSAOLightPass()
{
	this->SSAOcclusion.RenderLightPass();
}

ID3D11UnorderedAccessView& Scene::GetSSAOAccessView()
{
	return *this->SSAOcclusion.ssaoMapAccess;
}

void Scene::SwitchMenuMode(bool sw)
{
	this->menuMode = sw;
}

void Scene::ClearCullingObjects()
{
	this->staticObjects.clear();
}

int Scene::GetNrOfDynamicObjects() const
{
	return (int)this->dynamicObjects.size();
}

std::unordered_map<std::uintptr_t, MeshObject*>& Scene::GetAllStaticObjects()
{
	return this->staticObjects;
}

void Scene::TurnVisibilty(const int& index, float afterTime, bool visible)
{
	if (index < (int)this->objects.size() - 1 && index >= 0)
	{
		if (this->objects[index] != nullptr)
		{
			VisibleObject obj = {};
			obj.index = index;
			obj.finalTime = afterTime;
			obj.visible = visible;
			this->visibleObjects.push_back(obj);
		}
	}
}

void Scene::CheckObjectsVisibility(float deltaTime)
{
	if (!this->visibleObjects.empty())
	{
		for (size_t i = 0; i < this->visibleObjects.size(); i++)
		{
			//Increase the time of the object
			this->visibleObjects[i].currentTime += deltaTime;

			//Reached the end of the timer - going to set its visibility to on or off
			if (this->visibleObjects[i].currentTime >= this->visibleObjects[i].finalTime)
			{
				UINT index = visibleObjects[i].index;
				this->objects[index]->SetVisible(this->visibleObjects[i].visible);
				this->visibleObjects.erase(this->visibleObjects.begin() + i);
				
				//Check if any light is close
				bool foundLight = false;
				for (size_t lightIndex = 0; lightIndex < this->lights.size() && !foundLight; lightIndex++)
				{
					if (this->objects[index]->GetDistance(this->lights[lightIndex].position) < 5.0f)
					{
						foundLight = true;
						//Turn of the light - aka set type to anything other than 
						//0,1,2 (direction, point, spot)
						this->lights[lightIndex].attentuate.w = -1.0f;
						UpdateLightsBuffer();
					}
				}
			}
		}
	}
}

#ifdef _DEBUG
void Scene::RenderStaticBoundingBoxes()
{
	for (auto object : this->staticObjects)
	{
		object.second->RenderBoundingBoxes();
	}
}

void Scene::RenderDynamicBoundingBoxes()
{
	for (int i = 0; i < (int)this->dynamicObjects.size(); i++)
	{
		this->dynamicObjects[i]->RenderBoundingBoxes();
	}
}
#endif
