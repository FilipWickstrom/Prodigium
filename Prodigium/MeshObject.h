#pragma once
#include <string>
#include "GameObject.h"
#include "Mesh.h"
#include "Texture.h"
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
	DirectX::BoundingSphere boundingSphere;
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
	Collider modelCollider;

private:
	bool LoadTextures(bool isPlayer = false, bool isMonster = false);	//Super ugly...
	void BuildCollider(const DirectX::SimpleMath::Vector3& min, const DirectX::SimpleMath::Vector3& max, Collider& out);
	void BuildRenderCollider(const DirectX::SimpleMath::Vector3& min, const DirectX::SimpleMath::Vector3& max, const std::vector<DirectX::SimpleMath::Vector3>& positions, Collider& out);
	bool LoadColliders(bool hasColliders);
	
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
					bool hasBounds = true,
					bool hasAnimation = false,
					const DirectX::SimpleMath::Vector3& pos = {0.0f,0.0f,0.0f},
					const DirectX::SimpleMath::Vector3& rot = { 0.0f,0.0f,0.0f },
					const DirectX::SimpleMath::Vector3& scl= {1.0f,1.0f,1.0f});

	void SetVisible(bool toggle = true);
	void SetPickUp(bool toggle = true);

	//Rendering to display
	void Render();
	void RenderShadows();

	void UpdateBoundingBoxes();
	void UpdateRenderCollider();
	void UpdateBoundingBoxes(const DirectX::SimpleMath::Matrix& transform);
	const bool IsVisible() const;
	
	#ifdef _DEBUG
	void RenderBoundingBoxes();
	#endif

	void RemoveColliders();

	//Animation specific
	void ChangeAnimState(AnimationState state);
	void InterpolateAnim(bool toggle = true);
	bool HasAnimationEnded();
	AnimationState GetAnimState();
};
