#pragma once
#include "MeshObject.h"
#include "QuadTree.h"

using namespace DirectX::SimpleMath;

class Frustum
{
private:
	DirectX::BoundingFrustum transformed;
	DirectX::BoundingFrustum frustumColliderOriginal;

#ifdef _DEBUG
	ID3D11Buffer* vBuffer;
	ID3D11Buffer* iBuffer;

	bool CreateVertIndiBuffers();
#endif

public:
	Frustum();
	Frustum(const Frustum& other);
	virtual ~Frustum();

	// Has to run this function for every frame becayuse it is dependant on 
	// the currents frame view/projection matrix which will be altered next frame.
	void Update();
	// Will check intersection with objects to the frustum, will out a vector of all 
	// meshobjects that should be drawed current frame.
	void Drawable(QuadTree*& quadTree, std::vector<MeshObject*>& out);
	bool Initialize();

#ifdef _DEBUG
	void Render();
#endif
};