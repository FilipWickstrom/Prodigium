#pragma once
#include <d3d11.h>
#include "RenderTypes.h"

class RenderPass
{
private:
	RenderType type;
	bool CreateGeometryPass();
	bool CreateLightPass();
	bool CreatePostProcessPass();

public:
	RenderPass(RenderType type);
	virtual ~RenderPass();

	bool Initialize();
};