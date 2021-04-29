#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include "Graphics.h"
class GameObject
{
private:

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Vector3 rotation;

	// World Matrix, Pos, Scale, Rotation
	DirectX::SimpleMath::Matrix modelMatrix;

	// Buffer containing world matrix, updated each time BuildMatrix is run.
	ID3D11Buffer* modelMatrixBuffer;

public:

	GameObject();
	virtual ~GameObject();

	bool BuildMatrix(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scl, DirectX::SimpleMath::Vector3 rot);
	bool UpdateMatrix(DirectX::SimpleMath::Vector3 pos, DirectX::SimpleMath::Vector3 scl, DirectX::SimpleMath::Vector3 rot);
	bool UpdateMatrix();

	DirectX::SimpleMath::Vector3 GetPosition() const;
	DirectX::SimpleMath::Vector3 GetScale() const;
	DirectX::SimpleMath::Vector3 GetRotation() const;

	void SetPosition(DirectX::SimpleMath::Vector3 newPos);
	void SetScale(DirectX::SimpleMath::Vector3 newScale);
	void SetRotation(DirectX::SimpleMath::Vector3 newRotation);

	// Returns the float4x4 of the modelMatrix.
	const DirectX::SimpleMath::Matrix& GetModelMatrix();

	// Returns the transposed XMMATRIX of modelMatrix.
	DirectX::SimpleMath::Matrix GetTransposedMatrix() const;

	// Buffer for buffering buffer usage.
	ID3D11Buffer*& GetModelMatrixBuffer();

	float GetDistance(GameObject otherObject) const;

};