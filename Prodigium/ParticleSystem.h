#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"
#include <vector>
#define MAX_PARTICLES 1024

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

	std::vector<ParticleVertex> parts;

	bool hasSetup;

	// Makes the final preparations and does the draw call
	void InternalRender();

	// Clear any COM object before setting up particles.
	void ClearHistory();
public:

	ParticleSystem();
	virtual ~ParticleSystem();
	bool SetUp();

	void Render();
};