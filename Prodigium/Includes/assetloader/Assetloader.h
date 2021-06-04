#pragma once
#include "MFF.h"
#include <string>

namespace AssetLoader
{
	//Loads the model into memory
	bool LoadModel(const char* filePath);

	//Returns the loaded models mesh data
	const MyFileFormat::Mesh& GetModel(unsigned int modelId);

	//Returns how many meshes there is in the loaded scene
	const int GetNumberOfMeshesInScene();

	//Returns the loaded models vertex data
	const std::vector<MyFileFormat::VertexData>& GetVertexSet(unsigned int modelId);
}
