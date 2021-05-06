#pragma once
#include "SceneHandler.h"
#include <fstream>

class DebugInfo
{
private:
	DebugInfo();
	virtual ~DebugInfo();

	static DebugInfo* instance;
	ID3D11InputLayout* inputLayout;
	ID3D11VertexShader* vShader;
	ID3D11PixelShader* pShader;
	ID3D11RasterizerState* backFaceCullingOffState;
	std::string vShaderByteCode;

	bool CreateInputLayout();
	bool LoadShaders();
	bool CreateRasterizerState();

public:
	static bool Initialize();
	static void Destroy();
	static void Prepare();
	static void Clear();
};