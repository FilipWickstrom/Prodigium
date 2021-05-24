#pragma once
#include <d3d11.h>
#include "UsefulStructuresHeader.h"
#include <vector>
#include "GUIHandler.h"

#define MAX_PARTICLES 32768 * 6

class ParticleSystem
{
private:

	ID3D11GeometryShader* geoShader;
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11ComputeShader* computeShader;
	ID3D11BlendState* alphaBlendState;

	ID3D11Buffer* particleBuff;
	ID3D11UnorderedAccessView* particleAccess;
	ID3D11ShaderResourceView* particleView;
	std::string vertexData;

	ID3D11Buffer* speedBuffer;

	bool hasSetup;
	bool isActive;

private:

	// Shaders
	bool LoadVertexShader();
	bool LoadGeometryShader();
	bool LoadPixelShader();
	bool LoadComputeShader();

	// Makes the final preparations and does the draw call
	void InternalRender();

	// Remove any binded COM object onto pipeline.
	void Clear();

	// setup
	bool SetUp();
public:

	ParticleSystem();
	virtual ~ParticleSystem();
	void SetActive(bool act = true);
	bool UpdateSpeedBuffer(DirectX::SimpleMath::Vector3 playerPos, DirectX::SimpleMath::Vector3 monsterPos);
	void Render();
	const bool IsActive() const;
};

