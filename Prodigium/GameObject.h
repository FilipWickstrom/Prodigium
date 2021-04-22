#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include "Graphics.h"
using namespace DirectX;
class GameObject
{
private:

	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT3 rotation;

	// World Matrix, Pos, Scale, Rotation
	DirectX::XMFLOAT4X4 modelMatrix;

	// Buffer containing world matrix, updated each time BuildMatrix is run.
	ID3D11Buffer* modelMatrixBuffer;

public:

	GameObject();
	virtual ~GameObject();

	bool BuildMatrix(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scl, DirectX::XMFLOAT3 rot);
	bool UpdateMatrix(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 scl, DirectX::XMFLOAT3 rot);

	DirectX::XMFLOAT3 GetPosition() const;
	DirectX::XMFLOAT3 GetScale() const;
	DirectX::XMFLOAT3 GetRotation() const;

	// Returns the float4x4 of the modelMatrix.
	DirectX::XMFLOAT4X4 GetModelMatrix() const;

	// Returns the transposed XMMATRIX of modelMatrix.
	DirectX::XMMATRIX GetTransposedMatrix() const;

	// Buffer for buffering buffer usage.
	ID3D11Buffer*& GetModelMatrixBuffer();

	float GetDistance(GameObject otherObject) const;

};