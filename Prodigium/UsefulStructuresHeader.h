#pragma once
#include <DirectXMath.h>
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
	ID3D11ShaderResourceView* shaderResources[BUFFER_COUNT];
	ID3D11Texture2D* textures[BUFFER_COUNT];
};