#include "Frustum.h"

Frustum::Frustum()
{
}

Frustum::Frustum(const Frustum& other)
{
	for (int i = 0; i < NR_OF_PLANES; i++)
	{
		this->planes[i] = other.planes[i];
	}
}

Frustum::~Frustum()
{
}

void Frustum::BuildFrustum(float farZ, DirectX::SimpleMath::Matrix projMatrix, DirectX::SimpleMath::Matrix viewMatrix)
{
	float nearZ, r;
	Matrix frustumMatrix;

	// Calculate the minimum Z distance in the frustum.
	nearZ = -projMatrix._43 / projMatrix._33;
	r = farZ / (farZ - nearZ);
	projMatrix._33 = r;
	projMatrix._43 = -r * nearZ;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	frustumMatrix = viewMatrix * projMatrix;

	// Calculate near plane of frustum.
	planes[0].x = frustumMatrix._14 + frustumMatrix._13;
	planes[0].y = frustumMatrix._24 + frustumMatrix._23;
	planes[0].z = frustumMatrix._34 + frustumMatrix._33;
	planes[0].w = frustumMatrix._44 + frustumMatrix._43;
	planes[0].Normal().Normalize();

	// Calculate far plane of frustum.
	planes[1].x = frustumMatrix._14 - frustumMatrix._13;
	planes[1].y = frustumMatrix._24 - frustumMatrix._23;
	planes[1].z = frustumMatrix._34 - frustumMatrix._33;
	planes[1].w = frustumMatrix._44 - frustumMatrix._43;
	planes[1].Normal().Normalize();

	// Calculate left plane of frustum.
	planes[2].x = frustumMatrix._14 + frustumMatrix._11;
	planes[2].y = frustumMatrix._24 + frustumMatrix._21;
	planes[2].z = frustumMatrix._34 + frustumMatrix._31;
	planes[2].w = frustumMatrix._44 + frustumMatrix._41;
	planes[2].Normal().Normalize();

	// Calculate right plane of frustum.
	planes[3].x = frustumMatrix._14 - frustumMatrix._11;
	planes[3].y = frustumMatrix._24 - frustumMatrix._21;
	planes[3].z = frustumMatrix._34 - frustumMatrix._31;
	planes[3].w = frustumMatrix._44 - frustumMatrix._41;
	planes[3].Normal().Normalize();

	// Calculate top plane of frustum.
	planes[4].x = frustumMatrix._14 - frustumMatrix._12;
	planes[4].y = frustumMatrix._24 - frustumMatrix._22;
	planes[4].z = frustumMatrix._34 - frustumMatrix._32;
	planes[4].w = frustumMatrix._44 - frustumMatrix._42;
	planes[4].Normal().Normalize();

	// Calculate bottom plane of frustum.
	planes[5].x = frustumMatrix._14 + frustumMatrix._12;
	planes[5].y = frustumMatrix._24 + frustumMatrix._22;
	planes[5].z = frustumMatrix._34 + frustumMatrix._32;
	planes[5].w = frustumMatrix._44 + frustumMatrix._42;
	planes[5].Normal().Normalize();
}