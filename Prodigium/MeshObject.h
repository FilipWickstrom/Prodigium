#pragma once
#include <string>
#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"

constexpr int MAXNROFTEXTURES = 2;

class MeshObject : public GameObject
{
private:
	Mesh* mesh;
#ifdef _DEBUG
	std::vector<ID3D11Buffer*>boundingBoxBuffer;
#endif
	std::vector<Collider> collidersOriginal;
	Collider modelColliderOriginal;

	//Holds all the views of the textures:
	//1. Diffuse texture
	//2. Normal map
	ID3D11ShaderResourceView* shaderResourceViews[MAXNROFTEXTURES];

	bool isPickUp;
	bool isVisible;
	ID3D11Buffer* hasNormalMapBuffer;

private:
	bool BindTextureToSRV(ID3D11Texture2D*& texture, ID3D11ShaderResourceView*& srv);
	// Copies the mesh colliders
	void SetColliders();
	void SetRenderColliders();
	// Updates the planes of a boundingBox to account for rotation
	void UpdateBoundingPlanes();
	bool SetUpNormalMapBuffer();
#ifdef _DEBUG
	bool CreateColliderBuffers();
#endif

public:

	MeshObject();
	virtual ~MeshObject();

	bool Initialize(std::string meshObject, std::string diffuseTxt = "", std::string normalTxt = "", bool hasBounds = true,
					DirectX::SimpleMath::Vector3 pos = {0.0f,0.0f,0.0f}, DirectX::SimpleMath::Vector3 rot = { 0.0f,0.0f,0.0f }, 
					DirectX::SimpleMath::Vector3 scl= {1.0f,1.0f,1.0f});

	void SetVisible(bool toggle = true);
	void SetPickUp(bool toggle = true);
	void Render();
	void UpdateBoundingBoxes();
	void UpdateRenderBoundingBox();
	void UpdateBoundingBoxes(const DirectX::SimpleMath::Matrix& transform);
	const bool IsVisible() const;
#ifdef _DEBUG
	void RenderBoundingBoxes();
#endif

	std::vector<Collider> collidersOriginal;
	std::vector<Collider> colliders;
	Collider modelCollider;

	void RemoveColliders();
};