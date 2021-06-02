#pragma once
#include "MyFileFormat.h"
#include <vector>
#include <string>
#include <unordered_map>

namespace AssetLoader
{

	bool LoadModel(const char* filePath);

	std::string GetTextureName(unsigned int modelId);

	const MyFileFormat::Mesh GetModel(unsigned int modelId);

	const int GetNumberOfMeshesInScene();

	const std::unordered_map<unsigned int, MyFileFormat::VertexData> GetVertexSet(unsigned int modelId);
}
