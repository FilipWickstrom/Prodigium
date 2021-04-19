#include "RenderPass.h"

bool RenderPass::CreateGeometryPass()
{
	return true;
}

bool RenderPass::CreateLightPass()
{
	return true;
}

bool RenderPass::CreatePostProcessPass()
{
	return true;
}

RenderPass::RenderPass(RenderType type)
{
	this->type = type;
}

RenderPass::~RenderPass()
{

}

bool RenderPass::Initialize()
{
	switch (this->type)
	{
	case RenderType::GEOMETRY:
		if (!this->CreateGeometryPass())
		{
			return false;
		}
		break;
	case RenderType::LIGHTING:
		if (this->CreateLightPass())
		{
			return false;
		}
		break;
	case RenderType::POSTPROCESS:
		if (this->CreatePostProcessPass())
		{
			return false;
		}
		break;
	default:
		return false;
		break;
	};

	return true;
}
