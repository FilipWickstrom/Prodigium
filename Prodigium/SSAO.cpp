#include "SSAO.h"
#include <iostream>
#include <fstream>
#include "Graphics.h"
#include "UsefulStructuresHeader.h"

static float RandF()
{
	return (float)(rand()) / (float)RAND_MAX;
}

static float RandF(float a, float b)
{
	return a + RandF() * (b - a);
}

using namespace DirectX::SimpleMath;
bool SSAO::Setup()
{
	if (!this->LoadShaders())
		std::cout << "Loading Shaders in SSAO FAILED!\n";

	if (!this->SetupTexture())
		std::cout << "Setting up Texture FAILED!\n";

	if (!this->SetupPreparations())
		std::cout << "Setting up preparations for SSAO FAILED\n";

    this->hasSetup = true;
    return true;
}

void SSAO::InternalRender()
{
	ID3D11DepthStencilView* nullDepth = nullptr;
	// Prepare Phase
	UINT offset = 0;
	UINT stride = sizeof(QuadVertex);
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	Graphics::GetContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	Graphics::GetContext()->IASetInputLayout(this->inputQuad);
	Graphics::GetContext()->PSSetShader(this->pixelShader, nullptr, 0);
	Graphics::GetContext()->VSSetShader(this->vertexShader, nullptr, 0);
	Graphics::GetContext()->OMSetRenderTargets(1, &this->SSAORenderTarget, nullDepth);
	
	Graphics::GetContext()->PSSetShaderResources(1, 1, &this->randomVecShaderView);
	Graphics::GetContext()->PSSetSamplers(0, 1, &this->sampler);
	Graphics::GetContext()->PSSetConstantBuffers(0, 1, &this->ssaoBuffer);
	Graphics::GetContext()->VSSetConstantBuffers(1, 1, &this->ssaoBuffer);

	// Render Phase
	Graphics::GetContext()->DrawIndexed(6, 0, 0);

	this->Clear();
}

void SSAO::Clear()
{
	ID3D11DepthStencilView* nullStencil = nullptr;
	ID3D11ShaderResourceView* nullResView = nullptr;
	ID3D11Buffer* nullBuff = nullptr;
	ID3D11RenderTargetView* nullTargets[BUFFER_COUNT] = { nullptr };
	Graphics::GetContext()->OMSetRenderTargets(BUFFER_COUNT, nullTargets, nullStencil);
	Graphics::GetContext()->PSSetShaderResources(6, 1, &nullResView);
	Graphics::GetContext()->PSSetShaderResources(0, 1, &nullResView);
	Graphics::GetContext()->PSSetShaderResources(1, 1, &nullResView);

	Graphics::GetContext()->PSSetConstantBuffers(0, 1, &nullBuff);
	Graphics::GetContext()->PSSetConstantBuffers(10, 1, &nullBuff);
}

bool SSAO::LoadShaders()
{
	HRESULT hr;

	std::string shaderData;
	std::ifstream reader;

	reader.open("SSAOVertexShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &this->vertexShader);

	if (FAILED(hr))
	{
		return false;
	}

	this->vertexData = shaderData;
	shaderData.clear();
	reader.close();

	// PixelShaderDeferred
	reader.open("SSAOPixelShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)),
		std::istreambuf_iterator<char>());

	hr = Graphics::GetDevice()->CreatePixelShader(shaderData.c_str(), shaderData.length(), nullptr, &this->pixelShader);

	if (FAILED(hr))
	{
		return false;
	}

	shaderData.clear();
	reader.close();

	return true;
}

bool SSAO::SetupTexture()
{
	HRESULT hr;
	D3D11_TEXTURE2D_DESC tDesc;
	tDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tDesc.Width = Graphics::GetWindowWidth();
	tDesc.Height = Graphics::GetWindowHeight();
	tDesc.ArraySize = 1;
	tDesc.MipLevels = 1;
	tDesc.SampleDesc.Count = 1;
	tDesc.SampleDesc.Quality = 0;
	tDesc.Usage = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	tDesc.CPUAccessFlags = 0;
	tDesc.MiscFlags = 0;

	hr = Graphics::GetDevice()->CreateTexture2D(&tDesc, NULL, &this->SSAOMap);

	if (!FAILED(hr))
	{
		hr = Graphics::GetDevice()->CreateShaderResourceView(this->SSAOMap, NULL, &this->SSAOResView);
		hr = Graphics::GetDevice()->CreateRenderTargetView(this->SSAOMap, NULL, &this->SSAORenderTarget);
	}

	return !FAILED(hr);
}

bool SSAO::SetupPreparations()
{
	/*
	
		INPUT LAYOUT

	*/

	HRESULT result;
	D3D11_INPUT_ELEMENT_DESC input[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"INDEX", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};

	result = Graphics::GetDevice()->CreateInputLayout(input, 3, this->vertexData.c_str(),
		this->vertexData.length(), &this->inputQuad);
	assert(SUCCEEDED(result));

	/*
	
		VERTEX BUFFER

	*/

	QuadVertex verts[4] =
	{
		// Pos              Index               UV
		{{-1.0f, 1.0f, 0.0f},{1.0f, (float)corners, (float)samples},{0.0f, 0.0f}}, // Top Left
		{{1.0f, 1.0f, 0.0f},{2.0f, (float)corners, (float)samples},{1.0f, 0.0f}}, // Top Right
		{{1.0f, -1.0f, 0.0f},{3.0f, (float)corners, (float)samples},{1.0f, 1.0f}}, // Bottom Right
		{{-1.0f, -1.0f, 0.0f},{0.0f, (float)corners, (float)samples},{0.0f, 1.0f}} // Bottom Left
	};

	D3D11_BUFFER_DESC bufferDesc;
	bufferDesc.ByteWidth = sizeof(QuadVertex) * 4;
	bufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &(verts[0]);
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	result = Graphics::GetDevice()->CreateBuffer(&bufferDesc, &data, &this->vertexBuffer);
	if (FAILED(result))
		return false;

	/*
	
		INDEX BUFFER
	
	*/

	UINT indices[6] =
	{
		1,2,3,
		0,1,3
	};

	bufferDesc.ByteWidth = sizeof(UINT) * 6;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA Idata;
	Idata.pSysMem = &(indices[0]);

	result = Graphics::GetDevice()->CreateBuffer(&bufferDesc, &Idata, &this->indexBuffer);
	if (FAILED(result))
		return false;


	/*
	
		View Frustum Vectors for each corner and vectors going in different directions.
	
	*/
	float fov = DirectX::XM_PI * 0.5f;
	float farZ = 500.0f;

	float aspect = (float)Graphics::GetWindowWidth() / (float)Graphics::GetWindowHeight();

	float halfHeight = farZ * tanf(0.5f * fov);
	float halfWidth = aspect * halfHeight;

	this->frustumCorners[0] = DirectX::XMFLOAT4(-halfWidth, -halfHeight, farZ, 0.0f);
	this->frustumCorners[1] = DirectX::XMFLOAT4(-halfWidth, halfHeight, farZ, 0.0f);
	this->frustumCorners[2] = DirectX::XMFLOAT4(halfWidth, halfHeight, farZ, 0.0f);
	this->frustumCorners[3] = DirectX::XMFLOAT4(halfWidth, -halfHeight, farZ, 0.0f);

	DirectX::XMFLOAT4 vec[samples];

	vec[0] = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 0.0f);
	vec[1] = DirectX::XMFLOAT4(-1.0f, -1.0f, -1.0f, 0.0f);
	vec[2] = DirectX::XMFLOAT4(-1.0f, 1.0f, 1.0f, 0.0f);
	vec[3] = DirectX::XMFLOAT4(1.0f, -1.0f, -1.0f, 0.0f);
	vec[4] = DirectX::XMFLOAT4(1.0f, 1.0f, -1.0f, 0.0f);
	vec[5] = DirectX::XMFLOAT4(-1.0f, -1.0f, 1.0f, 0.0f);
	vec[6] = DirectX::XMFLOAT4(-1.0f, 1.0f, -1.0f, 0.0f);
	vec[7] = DirectX::XMFLOAT4(1.0f, -1.0f, 1.0f, 0.0f);

	vec[8] = DirectX::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
	vec[9] = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
	vec[10] = DirectX::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
	vec[11] = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	vec[12] = DirectX::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
	vec[13] = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

	for (int i = 0; i < samples; i++)
	{
		float randL = RandF(0.05f, 1.0f);
		// Scale vector with the randomized scalar
		Vector4 v = DirectX::XMVector4Normalize(DirectX::XMVectorScale(DirectX::XMLoadFloat4(&vec[i]), randL));
		DirectX::XMStoreFloat4(&this->randomVectors[i], v);
	}

	/*
		TEXTURE2D CONTAINING RANDOM VECTORS
	*/

	unsigned int thickness = Graphics::GetWindowWidth() * Graphics::GetWindowHeight();
	DirectX::XMFLOAT4* vecL = new DirectX::XMFLOAT4[thickness];
	for (unsigned int i = 0; i < thickness; i++)
	{
		vecL[i] = DirectX::XMFLOAT4(RandF(-1.0f, 1.0f),
			RandF(-1.0f, 1.0f), RandF(0.0f, 1.0f), 1.0f);
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = Graphics::GetWindowWidth();
	desc.Height = Graphics::GetWindowHeight();
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Format = DXGI_FORMAT_R8G8B8A8_SNORM;
	desc.Usage = D3D11_USAGE_IMMUTABLE;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.ArraySize = 1;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vData;
	vData.pSysMem = &(vecL[0]);
	vData.SysMemPitch = Graphics::GetWindowWidth() * 4;
	vData.SysMemSlicePitch = 0;

	result = Graphics::GetDevice()->CreateTexture2D(&desc, &vData, &this->randomVecTexture);
	if (FAILED(result))
	{
		delete[] vecL;
		return false;
	}
	else
		result = Graphics::GetDevice()->CreateShaderResourceView(this->randomVecTexture, NULL, &this->randomVecShaderView);
	delete[] vecL;

	/*
	
		SAMPLER STATE

	*/

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0;
	samplerDesc.MaxAnisotropy = 16;
	samplerDesc.BorderColor[0] = samplerDesc.BorderColor[1] = samplerDesc.BorderColor[2] = samplerDesc.BorderColor[3] = 1e5f;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = Graphics::GetDevice()->CreateSamplerState(&samplerDesc, &this->sampler);

	/*

	SSAO Buffer Containing frustumCorners and randomVectors

	*/

	SSAOBuffer sBuff;
	for (int i = 0; i < corners; i++)
	{
		sBuff.frustumCorners[i] = this->frustumCorners[i];
	}
	for (int i = 0; i < samples; i++)
	{
		sBuff.randomVectors[i] = this->randomVectors[i];
	}
	D3D11_BUFFER_DESC bDesc;
	bDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bDesc.Usage = D3D11_USAGE_IMMUTABLE;
	bDesc.CPUAccessFlags = 0;
	bDesc.MiscFlags = 0;
	bDesc.ByteWidth = sizeof(SSAOBuffer);

	data.pSysMem = &(sBuff);
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	result = Graphics::GetDevice()->CreateBuffer(&bDesc, &data, &this->ssaoBuffer);

	/*
	
		UNORDERED ACCESS VIEW
	
	*/

	result = Graphics::GetDevice()->CreateUnorderedAccessView(this->SSAOMap, NULL, &this->ssaoMapAccess);

	return !FAILED(result);
}

SSAO::SSAO()
{
    this->inputQuad = nullptr;
    this->vertexBuffer = nullptr;
    this->SSAOMap = nullptr;
    this->SSAORenderTarget = nullptr;
    this->SSAOResView = nullptr;
    this->pixelShader = nullptr;
    this->vertexShader = nullptr;
	this->indexBuffer = nullptr;
	this->randomVecTexture = nullptr;
	this->randomVecShaderView = nullptr;
	this->ssaoBuffer = nullptr;
	this->ssaoMapAccess = nullptr;

    this->vertexData = "";
    this->hasSetup = false;
}

SSAO::~SSAO()
{
    if (this->inputQuad)
        this->inputQuad->Release();
    if (this->vertexBuffer)
        this->vertexBuffer->Release();
    if (this->vertexShader)
        this->vertexShader->Release();
    if (this->pixelShader)
        this->pixelShader->Release();
    if (this->SSAORenderTarget)
        this->SSAORenderTarget->Release();
    if (this->SSAOResView)
        this->SSAOResView->Release();
    if (this->SSAOMap)
        this->SSAOMap->Release();
	if (this->indexBuffer)
		this->indexBuffer->Release();
	if (this->randomVecTexture)
		this->randomVecTexture->Release();
	if (this->randomVecShaderView)
		this->randomVecShaderView->Release();
	if (this->ssaoBuffer)
		this->ssaoBuffer->Release();
	if (this->sampler)
		this->sampler->Release();
	if (this->ssaoMapAccess)
		this->ssaoMapAccess->Release();
}

void SSAO::Render()
{
    if (!this->hasSetup)
    {
        this->Setup();
    }
    else
        this->InternalRender();
}

void SSAO::RenderLightPass()
{
	Graphics::GetContext()->PSSetShaderResources(6, 1, &this->SSAOResView);
}
