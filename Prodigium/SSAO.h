#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <string>

constexpr int samples = 14;
constexpr int corners = 4;

struct QuadVertex
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 farPlaneIndex;
	DirectX::SimpleMath::Vector2 uv;
};

struct SSAOBuffer
{
	DirectX::SimpleMath::Vector4 frustumCorners[corners];
	DirectX::SimpleMath::Vector4 randomVectors[samples];
};

class SSAO
{
private:

	ID3D11InputLayout* inputQuad;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	ID3D11Buffer* ssaoBuffer;
	std::string vertexData;

	ID3D11Texture2D* SSAOMap;
	ID3D11ShaderResourceView* SSAOResView; // Resource View for SSAOMap
	ID3D11RenderTargetView* SSAORenderTarget; // Render target view for SSAOMap

	ID3D11Texture2D* randomVecTexture;
	ID3D11ShaderResourceView* randomVecShaderView;
	ID3D11SamplerState* sampler;

	DirectX::SimpleMath::Vector4 frustumCorners[corners];
	DirectX::SimpleMath::Vector4 randomVectors[samples];

	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;

	bool Setup();
	bool hasSetup;
	bool LoadShaders();
	bool SetupTexture();
	bool SetupPreparations();

	// Setup and Render
	void InternalRender();

	// Release all binded objects
	void Clear();

public:

	SSAO();
	~SSAO();

	void Render();
};