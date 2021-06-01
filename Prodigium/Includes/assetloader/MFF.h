#pragma once
#include "MyFileFormat.h"
#include <vector>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <filesystem>

class MFF
{
private:
	unsigned int meshId = 0;
	unsigned int materialId = 0;

	std::string m_readPath;
	std::ifstream m_readFile;
	MyFileFormat::FilePath m_filePath;
	MyFileFormat::Scene m_scene;

	std::unordered_map<unsigned int, MyFileFormat::Mesh> m_meshHeader;
	std::unordered_map<unsigned int, MyFileFormat::Material> m_materialHeader;
	std::unordered_map<unsigned int, MyFileFormat::VertexData> m_meshVertices;
	std::unordered_map<unsigned int, std::unordered_map<unsigned int, MyFileFormat::VertexData>> m_meshVerticesSets;

	template<typename T>
	void ReadHeader(T* header);
	void ReadFilePath();
	void ReadScene();
	void ReadMesh();
	void ReadMaterial();
	void ResetObject();

public:
	MFF();
	~MFF();
	void SetFilePath(const char* p_Path);
	bool ReadFile();
	void CopyTexturesToDirectory(std::string directory);

	static MFF& GetMFF();

	const MyFileFormat::Mesh& GetModel(unsigned int modelId);
	const MyFileFormat::Material& GetMaterial(unsigned int materialId);
	const int& GetNumberOfMeshesInScene();
	const std::string GetModelTextureName(unsigned int modelId);
	const std::unordered_map<unsigned int, MyFileFormat::VertexData>& GetVertexSet(unsigned int modelId);
};

template<typename T>
inline void MFF::ReadHeader(T* header)
{
	char* ptr = (char*)header;
	ptr += sizeof(MyFileFormat::SectionHeader);

	m_readFile.read(ptr, sizeof(T) - sizeof(MyFileFormat::SectionHeader));
}