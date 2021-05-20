#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
class GameObject
{
private:
	// Buffer containing world matrix, updated each time BuildMatrix is run.
	ID3D11Buffer* modelMatrixBuffer;

protected:
	// World Matrix, Pos, Scale, Rotation
	DirectX::SimpleMath::Matrix modelMatrix;

public:

	GameObject();
	virtual ~GameObject();

	DirectX::SimpleMath::Vector3 position;
	DirectX::SimpleMath::Vector3 scale;
	DirectX::SimpleMath::Vector3 rotation;
	DirectX::SimpleMath::Vector3 forward;
	DirectX::SimpleMath::Vector3 right;
	DirectX::SimpleMath::Vector3 up;

	bool CreateModelMatrixBuffer();
	bool BuildMatrix(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& scl, const DirectX::SimpleMath::Vector3& rot);
	bool UpdateMatrix(const DirectX::SimpleMath::Vector3& pos, const DirectX::SimpleMath::Vector3& scl, const DirectX::SimpleMath::Vector3& rot);
	bool UpdateMatrix();
	void UpdateMatrixCPU();
	
	const DirectX::SimpleMath::Vector3& GetPosition() const;
	const DirectX::SimpleMath::Vector3& GetScale() const;
	const DirectX::SimpleMath::Vector3& GetRotation() const;

	void SetPosition(const DirectX::SimpleMath::Vector3& newPos);
	void SetScale(const DirectX::SimpleMath::Vector3& newScale);
	void SetRotation(const DirectX::SimpleMath::Vector3& newRotation);

	// Returns the float4x4 of the modelMatrix.
	const DirectX::SimpleMath::Matrix& GetModelMatrix() const;
	float GetDistance(const DirectX::SimpleMath::Vector4& otherObject) const;
	// Returns the transposed XMMATRIX of modelMatrix.
	const DirectX::SimpleMath::Matrix GetTransposedMatrix() const;

	// Buffer for buffering buffer usage.
	ID3D11Buffer*& GetModelMatrixBuffer();

	float GetDistance(const GameObject &otherObject) const;
};