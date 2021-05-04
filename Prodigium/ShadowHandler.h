#pragma once
#include "ShadowMap.h"
#include <vector>
struct MatrixLight
{
	DirectX::SimpleMath::Matrix view;
	DirectX::SimpleMath::Matrix projection;
};
class ShadowHandler
{
private:

	std::vector<ShadowMap*> shadows;
	std::vector<MatrixLight> structs;
	ID3D11Texture2D* shadowMapArray;
	ID3D11Buffer* lightInfoBuffer;

	// Contains the view and projection matrices of our lights
	ID3D11ShaderResourceView* lightArrayView;

	// Is for sampling the texture2d array.
	ID3D11ShaderResourceView* arrayView;
	bool newMap;
	int nrOf;
public:

	bool SetupMapArray();
	ShadowHandler();
	void Add(const LightStruct& L);
	virtual ~ShadowHandler();

	ShadowMap& GetShadow(const int index);

	void Render(const int index);
	const int NrOfShadows() const;
	void RenderLightPass();
	void Clear();
};