#pragma once
#include <DirectXMath.h>
#include "Texture.h"
#define BUFFER_COUNT 3

/*
	Struct for usage with Vertices.
	Try to have it as general as possible for possible reusage.
*/
struct Vertex
{
	DirectX::XMFLOAT3 position = {};
	DirectX::XMFLOAT2 uv = {};
	DirectX::XMFLOAT3 normal = {};
};

/*
	Struct for usage with Materialized objects.
	Contains the mostly used ambient, diffuse and specular.
*/
struct Material
{
	DirectX::XMFLOAT4 ambient;
	DirectX::XMFLOAT4 diffuse;
	DirectX::XMFLOAT4 specular;
};

/*
	Struct for usage with different Matrices
	Contains the matrices for view and projection.
*/
struct Matrix
{
	DirectX::XMFLOAT4X4 view;
	DirectX::XMFLOAT4X4 projection;
};

/*
	Compact version of the 'Matrix' struct.
	Contains one element that can be used as world, view and projection in one matrix.
*/
struct MatrixCompact
{
	DirectX::XMFLOAT4X4 transform;
};

struct TextureRenderTargets
{
	ID3D11RenderTargetView* renderTargets[BUFFER_COUNT];
	ID3D11ShaderResourceView* shaderResourceViews[BUFFER_COUNT];
	Texture* textures[BUFFER_COUNT];
};

/*
	Structure for use of directional light, it only contains one
	XMFLOAT4 for which direction is should light up.
*/
struct DirectionalLight
{
	DirectX::XMFLOAT4 direction;
};

/*
	Structure for the use of Spotlight type of lighting. Contains
	Position, direction and attenuation.
*/
struct SpotLight
{
	DirectX::XMFLOAT4 position; // position.w is range.
	DirectX::XMFLOAT4 direction; // direction.w is Spot.
	DirectX::XMFLOAT4 attentuate;
};

/*
	Structure to simulate a Pointlight type of lighting. Contains
	position, range and attenuation.
*/
struct PointLight
{
	DirectX::XMFLOAT4 position; // position.w is range.
	DirectX::XMFLOAT4 attentuate;
};