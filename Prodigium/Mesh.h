#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "UsefulStructuresHeader.h"

class Mesh
{
private:

	Material objectMat;

	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* shaderResourceView;
	ID3D11Buffer* vertexBuffer;
	ID3D11InputLayout* inputLayout;
	ID3D11SamplerState* sampler;

	/*
		Different VertexShader for each object? Or globally used Vertex shader somehow? <- Might save performance somewhat.
		ID3D11VertexShader* vertexShader;
	*/


public:



};