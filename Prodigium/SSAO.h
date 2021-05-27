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

class SSAO
{
private:

	// todo: Random Vector Texture

	ID3D11InputLayout* inputQuad;
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	std::string vertexData;

	ID3D11Texture2D* SSAOMap;
	ID3D11ShaderResourceView* SSAOResView; // Resource View for SSAOMap
	ID3D11RenderTargetView* SSAORenderTarget; // Render target view for SSAOMap

	DirectX::SimpleMath::Vector4 frustumCorners[4];
	DirectX::SimpleMath::Vector4 randomVectors[14];

	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;

	bool Setup();
	bool hasSetup;
	bool LoadShaders();
	bool SetupTexture();
	bool SetupPreparations();

	void InternalRender();

public:

	SSAO();
	~SSAO();

	void Render();
};