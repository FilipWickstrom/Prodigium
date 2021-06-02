#pragma once
#include "MyFileFormat.h"
#include <vector>
#include <iostream>
#include <fstream>

class MFF
{
private:
	unsigned int meshId = 0;

	std::string m_readPath;
	std::ifstream m_readFile;
	MyFileFormat::FilePath m_filePath;
	MyFileFormat::Scene m_scene;

	std::vector<MyFileFormat::Mesh> m_meshHeader;
	std::vector<MyFileFormat::VertexData> m_meshVertices;
	std::vector<std::vector<MyFileFormat::VertexData>> m_meshVerticesSets;

	template<typename T>
	void ReadHeader(T* header);
	void ReadFilePath();
	void ReadScene();
	void ReadMesh();
	void ResetObject();

public:
	MFF();
	~MFF();
	void SetFilePath(const char* p_Path);
	bool ReadFile();
	void CopyTexturesToDirectory(std::string directory);

	static MFF& GetMFF();

	const MyFileFormat::Mesh& GetModel(unsigned int modelId);
	const int& GetNumberOfMeshesInScene();
	const std::vector<MyFileFormat::VertexData>& GetVertexSet(unsigned int modelId);
};

template<typename T>
inline void MFF::ReadHeader(T* header)
{
	char* ptr = (char*)header;
	ptr += sizeof(MyFileFormat::SectionHeader);

	m_readFile.read(ptr, sizeof(T) - sizeof(MyFileFormat::SectionHeader));
}