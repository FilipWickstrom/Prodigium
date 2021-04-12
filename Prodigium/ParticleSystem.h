#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"

class ParticleSystem
{
private:

	ID3D11GeometryShader* geoShader;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ComputeShader* computeShader;
	ID3D11InputLayout* inputLayout;
	ID3D11UnorderedAccessView* particleAccess;

	ID3D11Buffer* particleBuff;

public:

	ParticleSystem();
	~ParticleSystem();

};