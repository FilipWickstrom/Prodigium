#pragma once
#include <d3d11.h>
#include <string>
#include "UsefulStructuresHeader.h"
#include <fstream>

enum class RenderType
{
	GEOMETRY,
	LIGHTING,
	POSTPROCESS
};

class RenderPass
{
private:
	RenderType* type;
	bool CreateGeometryPass();
	bool CreateLightPass();
	bool CreatePostProcessPass();
	bool LoadShaders(bool VertexShader = false, bool PixelShader = false, bool GeometryShader = false, bool DomainShader = false, bool HullShader = false);

public:
	RenderPass();
	virtual ~RenderPass();

	bool Initialize(RenderType type);
	bool Prepare();
	bool Clear();
};