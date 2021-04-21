#include "RenderPass.h"

bool RenderPass::CreateGeometryPass()
{
	TextureRenderTargets gBuffer[BUFFER_COUNT] = { nullptr };
	ID3D11DepthStencilView* depthStencilView = nullptr;
	ID3D11Texture2D* depthTexture = nullptr;
	ID3D11InputLayout* inputLayout = nullptr;
	ID3D11VertexShader* vShader = nullptr;
	ID3D11PixelShader* pShader = nullptr;
	ID3D11SamplerState* sampler = nullptr;
	std::string vShaderByteCode;

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

bool RenderPass::LoadShaders(bool VertexShader, bool PixelShader, bool GeometryShader, bool DomainShader, bool HullShader)
{
	//HRESULT hr;

	if (VertexShader)
	{
		ID3D11VertexShader* vShader = nullptr;

		std::string shaderData;
		std::ifstream reader;

		reader.open("VertexShaderGPass.cso", std::ios::binary | std::ios::ate);
		if (!reader.is_open())
		{
			return false;
		}
		reader.seekg(0, std::ios::end);
		shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
		reader.seekg(0, std::ios::beg);

		shaderData.assign((std::istreambuf_iterator<char>(reader)),
			std::istreambuf_iterator<char>());

		/*hr = Engine::getDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader);*/

	/*	if (FAILED(hr))
		{
			return false;
		}*/

		std::string vShaderByteCode = shaderData;
		shaderData.clear();
		reader.close();
	}

	return true;
}

RenderPass::RenderPass()
{
	this->type = nullptr;
}

RenderPass::~RenderPass()
{

}

bool RenderPass::Initialize(RenderType type)
{
	if (this->type != nullptr)
	{
		return false;
	}

	this->type = new RenderType;
	*this->type = type;

	switch (*this->type)
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

bool RenderPass::Prepare()
{
	return false;
}

bool RenderPass::Clear()
{
	return false;
}
