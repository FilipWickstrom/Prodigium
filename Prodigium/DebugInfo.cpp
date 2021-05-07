#include "DebugInfo.h"

DebugInfo* DebugInfo::instance = nullptr;

DebugInfo::DebugInfo()
{
	this->inputLayout = nullptr;
	this->pShader = nullptr;
	this->vShader = nullptr;
	this->backFaceCullingOffState = nullptr;
	this->vShaderByteCode = "";
}

DebugInfo::~DebugInfo()
{
}

bool DebugInfo::Initialize()
{
	if (DebugInfo::instance == nullptr)
	{
		DebugInfo::instance = new DebugInfo();

		if (!DebugInfo::instance->LoadShaders())
		{
			return false;
		}

		if (!DebugInfo::instance->CreateInputLayout())
		{
			return false;
		}

		if (!DebugInfo::instance->CreateRasterizerState())
		{
			return false;
		}

		return true;
	}

	return false;
}

bool DebugInfo::CreateInputLayout()
{
	HRESULT hr;

	D3D11_INPUT_ELEMENT_DESC layout = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

	hr = Graphics::GetDevice()->CreateInputLayout(&layout, 1, vShaderByteCode.c_str(), vShaderByteCode.length(), &inputLayout);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool DebugInfo::CreateRasterizerState()
{
	D3D11_RASTERIZER_DESC razDesc = {};
	razDesc.FillMode = D3D11_FILL_SOLID;
	razDesc.CullMode = D3D11_CULL_NONE;
	razDesc.DepthClipEnable = TRUE;
	razDesc.FrontCounterClockwise = FALSE;
	razDesc.DepthBias = 0;
	razDesc.DepthBiasClamp = 0.f;
	razDesc.SlopeScaledDepthBias = 0.f;
	razDesc.ScissorEnable = false;
	razDesc.MultisampleEnable = false;
	razDesc.AntialiasedLineEnable = false;

	Graphics::GetDevice()->CreateRasterizerState(&razDesc, &backFaceCullingOffState);

	return true;
}

void DebugInfo::Prepare()
{
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	Graphics::GetContext()->VSSetShader(DebugInfo::instance->vShader, NULL, 0);
	Graphics::GetContext()->PSSetShader(DebugInfo::instance->pShader, NULL, 0);
	Graphics::GetContext()->IASetInputLayout(DebugInfo::instance->inputLayout);
	Graphics::GetContext()->RSSetState(DebugInfo::instance->backFaceCullingOffState);
}

bool DebugInfo::LoadShaders()
{
	HRESULT hr;

	std::string shaderData;
	std::ifstream reader;

	reader.open("DebugVertexShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &vShader);

	if (FAILED(hr))
	{
		return false;
	}

	this->vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();

	// PixelShaderDeferred
	reader.open("DebugPixelShader.cso", std::ios::binary | std::ios::ate);

	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = Graphics::GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &pShader);

	if (FAILED(hr))
	{
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

void DebugInfo::Clear()
{
	ID3D11VertexShader* vShaderNull = nullptr;
	ID3D11PixelShader* pShaderNull = nullptr;
	ID3D11InputLayout* inputLayoutNull = nullptr;
	ID3D11RasterizerState* rasterizerStateNull = nullptr;

	Graphics::GetContext()->VSSetShader(vShaderNull, NULL, 0);
	Graphics::GetContext()->PSSetShader(pShaderNull, NULL, 0);
	Graphics::GetContext()->IASetInputLayout(inputLayoutNull);
	Graphics::GetContext()->RSSetState(rasterizerStateNull);
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void DebugInfo::Destroy()
{
	if (DebugInfo::instance->inputLayout)
		DebugInfo::instance->inputLayout->Release();
	if (DebugInfo::instance->pShader)
		DebugInfo::instance->pShader->Release();
	if (DebugInfo::instance->vShader)
		DebugInfo::instance->vShader->Release();
	if (DebugInfo::instance->backFaceCullingOffState)
		DebugInfo::instance->backFaceCullingOffState->Release();

	if (DebugInfo::instance != nullptr)
	{
		delete DebugInfo::instance;
	}
}