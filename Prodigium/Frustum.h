#pragma once
#include "Graphics.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

class Frustum
{
private:
	static const int NR_OF_PLANES = 6;
	DirectX::SimpleMath::Plane planes[NR_OF_PLANES];

public:
	Frustum();
	Frustum(const Frustum& other);
	virtual ~Frustum();

	// Has to run this function for every frame it is dependant on view/projection matrix
	// which will be altered every frame.
	void BuildFrustum(float farZ, DirectX::SimpleMath::Matrix projMatrix, DirectX::SimpleMath::Matrix viewMatrix);

};