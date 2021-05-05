#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"
#include <vector>
#define MAX_PARTICLES 4096

class ParticleSystem
{
private:

	ID3D11GeometryShader* geoShader;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ComputeShader* computeShader;

	ID3D11Buffer* particleBuff;
	ID3D11UnorderedAccessView* particleAccess;
	ID3D11ShaderResourceView* particleView;
	ID3D11DepthStencilState* defaultState;
	std::string vertexData;

	bool hasSetup;

private:

	// Shaders
	bool LoadVertexShader();
	bool LoadGeometryShader();
	bool LoadPixelShader();
	bool LoadComputeShader();

	// Makes the final preparations and does the draw call
	void InternalRender();

	// Clear any COM object before setting up particles.
	void ClearHistory();

	// Remove any binded COM object onto pipeline.
	void Clear();

public:

	ParticleSystem();
	virtual ~ParticleSystem();
	bool SetUp();

	void Render();
};

