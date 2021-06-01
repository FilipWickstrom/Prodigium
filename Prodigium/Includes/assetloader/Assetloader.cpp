#include "Assetloader.h"
#include "MFF.h"
#include <fstream>

bool AssetLoader::LoadModel(const char* p_Path)
{
	MFF::GetMFF().SetFilePath(p_Path);

	if (MFF::GetMFF().ReadFile() == false)
		return false;	
	else
		return true;
}

const std::string AssetLoader::GetTextureName(unsigned int modelId)
{
	return MFF::GetMFF().GetModelTextureName(modelId);
}

const MyFileFormat::Mesh AssetLoader::GetModel(unsigned int modelId)
{
	return MFF::GetMFF().GetModel(modelId);
}

const MyFileFormat::Material AssetLoader::GetMaterial(unsigned int materialId)
{
	return MFF::GetMFF().GetMaterial(materialId);
}

const int AssetLoader::GetNumberOfMeshesInScene()
{
	return MFF::GetMFF().GetNumberOfMeshesInScene();
}

const std::unordered_map<unsigned int, MyFileFormat::VertexData> AssetLoader::GetVertexSet(unsigned int modelId)
{
	return MFF::GetMFF().GetVertexSet(modelId);
}
