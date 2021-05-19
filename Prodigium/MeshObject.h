#pragma once
#include <string>
#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "AnimatedObject.h"

constexpr int MAXNROFTEXTURES = 2;

struct Collider
{
	struct Plane
	{
		DirectX::SimpleMath::Vector3 point;
		DirectX::SimpleMath::Vector3 normal;
	};
	Plane planes[4];
	DirectX::BoundingOrientedBox boundingBox;
};

class MeshObject : public GameObject
{
private:
	Mesh* mesh;
	#ifdef _DEBUG
	std::vector<ID3D11Buffer*>boundingBoxBuffer;
	#endif

	//Holds all the views of the textures:
	//1. Diffuse texture | 2. Normal map
	ID3D11ShaderResourceView* shaderResourceViews[MAXNROFTEXTURES];

	bool isPickUp;
	bool isVisible;
	ID3D11Buffer* hasNormalMapBuffer;

	//Animation
	AnimatedObject* animatedObj;
	bool isAnimated;

public: 
	std::vector<Collider> collidersOriginal;
	std::vector<Collider> colliders;

private:
	bool LoadTextures(std::string& diffuse, std::string& normal);
	void BuildColliders(const DirectX::SimpleMath::Vector3& min, const DirectX::SimpleMath::Vector3& max);
	bool LoadColliders();
	
	// Updates the planes of a boundingBox to account for rotation
	void UpdateBoundingPlanes();
	bool SetUpNormalMapBuffer();
	
	#ifdef _DEBUG
	bool CreateColliderBuffers();
	#endif

public:

	MeshObject();
	virtual ~MeshObject();

	bool Initialize(const std::string& meshObject, 
					std::string diffuse = "", 
					std::string normal = "", 
					bool hasBounds = true,
					bool hasAnimation = false,
					const DirectX::SimpleMath::Vector3& pos = {0.0f,0.0f,0.0f},
					const DirectX::SimpleMath::Vector3& rot = { 0.0f,0.0f,0.0f },
					const DirectX::SimpleMath::Vector3& scl= {1.0f,1.0f,1.0f});

	void SetVisible(bool toggle = true);
	void SetPickUp(bool toggle = true);

	void Render(bool shadowPass = false);
	void UpdateBoundingBoxes();
	void UpdateBoundingBoxes(const DirectX::SimpleMath::Matrix& transform);
	const bool IsVisible() const;
	
	#ifdef _DEBUG
	void RenderBoundingBoxes();
	#endif

	void RemoveColliders();

	void ChangeAnimState(AnimationState state);
};