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
	ID3D11VertexShader* vertexShader;
	D3D11_VIEWPORT viewPort;

	// Contains the view and projection matrices of our lights
	ID3D11ShaderResourceView* lightArrayView;

	// Is for sampling the texture2d array.
	ID3D11ShaderResourceView* arrayView;
	bool newMap;
	int nrOf;

	const bool LoadVertexShader();

	// if any of the COM objects exists we release them and create new ones.
	void ClearHistory();
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
	void Prepare();
};