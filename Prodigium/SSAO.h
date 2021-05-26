#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <string>

struct QuadVertex
{
	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 farPlaneIndex;
	DirectX::SimpleMath::Vector2 uv;
};

class SSAO
{
private:

	ID3D11InputLayout* inputQuad;
	ID3D11Buffer* vertexBuffer;
	std::string vertexData;

	ID3D11Texture2D* SSAOMap;
	ID3D11ShaderResourceView* SSAOResView; // Resource View for SSAOMap
	ID3D11RenderTargetView* SSAORenderTarget; // Render target view for SSAOMap

	ID3D11PixelShader* pixelShader;
	ID3D11VertexShader* vertexShader;

	bool Setup();

public:



};