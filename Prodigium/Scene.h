#pragma once
#include <vector>
#include "MeshObject.h"
#include "ShadowHandler.h"
#include "ParticleSystem.h"
#include "SSAO.h"

#define SHADOWRANGE 125.0f
struct InfoStruct
{
	DirectX::XMFLOAT4 info;
};

class Scene
{
private:

	// Vector for all the objects present in this scene.
	std::vector<MeshObject*> objects;
	std::unordered_map<std::uintptr_t, MeshObject*> visibleObjects;

	// points to the current selected object.
	int currentObject;

	// points to the current selected light.
	int currentLight;

	bool menuMode;

	/*
		For rendering lights
	*/

	std::vector<LightStruct> lights;
	// keeps track on if it should create buffer or not during render.
	bool firstTime;
	// contains information of all the lights.
	ID3D11Buffer* lightBuffer;
	ID3D11ShaderResourceView* lightShaderView;
	// contains general information of the lights, such as, amount.
	ID3D11Buffer* infoBuffer;

	const bool SetupLightBuffer();
	const bool UpdateInfoBuffer();

	/*
		For rendering Shadows
	*/
	ShadowHandler* shadowHandler;

	/*
		For rendering particles
	*/
	ParticleSystem particles;

	/*
		For rendering SSAO
	*/
	SSAO SSAOcclusion;

public:

	Scene();
	virtual ~Scene();

	// dont worry about this if you're in game.h making the game.
	const bool SetupInfoBuffer();

	// adds an object to the scene, current selected object will point towards this new object.
	void Add(const std::string& objFile,
			 const std::string& diffuseTxt = "", 
			 const std::string& normalTxt = "", 
			 bool hasBounds = true,
			 bool hasAnimation = false,
			 const DirectX::SimpleMath::Vector3& position = {0.0f, 0.0f, 0.0f},
			 const DirectX::SimpleMath::Vector3& rotation = {0.0f, 0.0f, 0.0f},
			 const DirectX::SimpleMath::Vector3& scale = {1.0f, 1.0f, 1.0f});

	// Adds a reference to an already initialized object to the scene
	void Add(MeshObject* object);

	/* 
	* adds a light into the scene, the behavior of this light is defined by the struct input.
	* for more information about this struct go into UsefulStructureHeader.h
	*/
	void AddLight(LightStruct& L);
	void PopLight();
	void PopAllLights();

	// update the object matrix buffer of current selected object, as in update the position, rotation and scale.
	void UpdateMatrix(const DirectX::SimpleMath::Vector3& pos = { 0.0f, 0.0f, 0.0f },
		const DirectX::SimpleMath::Vector3& rotation = { 0.0f, 0.0f, 0.0f }, const DirectX::SimpleMath::Vector3& scale = { 1.0f, 1.0f, 1.0f });

	// the vector will erase whatever item was at begin() + index, resets currentObject to 0.
	void RemoveObject(const int& index);

	// switches the current object to indexed object if it is within the scope of the vector! else nothing changes.
	void SwitchObject(const int& index);

	// return the object at index
	MeshObject& GetMeshObject(int index);

	const std::vector<MeshObject*>& GetAllMeshObjects();

	ShadowHandler& GetShadows();

	ParticleSystem& GetParticles();

	// return the number of objects inside the scene.
	const int GetNumberOfObjects() const;

	// will cycle through vector and pop_back() through all elements.
	void RemoveAllObjects();

	// runs a simple pop_back if so is desired.
	void Pop();

	// loop through all objects and call their render function.
	void Render();

	void Render(const std::unordered_map<std::uintptr_t, MeshObject*>& toRender);

	// loop through all lights are bind them to the light pass
	void RenderLights();

#ifdef _DEBUG
	void RenderBoundingBoxes(const std::unordered_map<std::uintptr_t, MeshObject*>& toRender);
#endif

	// render all shadows to be prepared for the light pass
	void RenderShadows();
	void RenderShadows(const std::unordered_map<std::uintptr_t, MeshObject*>& toRender);

	// render the particles inside the scene.
	void RenderParticles();

	void RenderSSAO();
	void RenderSSAOLightPass();

	void SwitchMenuMode(bool sw = true);

	void ClearCullingObjects();

	std::unordered_map<std::uintptr_t, MeshObject*>& GetAllCullingObjects();
};