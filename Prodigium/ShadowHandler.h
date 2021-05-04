#pragma once
#include "ShadowMap.h"
#include <vector>
class ShadowHandler
{
private:

	std::vector<ShadowMap*> shadows;
	ID3D11Texture2D* shadowMapArray;
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