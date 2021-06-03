#include "MFF.h"
#include <iostream>

void MFF::ReadFilePath()
{
	MyFileFormat::FilePath path;
	ReadHeader(&path);
	m_filePath = path;
}

void MFF::ReadScene()
{
	MyFileFormat::Scene scene;
	ReadHeader(&scene);
	m_scene = scene;
}

void MFF::ReadMesh()
{
	MyFileFormat::Mesh mesh;
	MyFileFormat::VertexData vertex;
	ReadHeader(&mesh);

	m_meshHeader.push_back(mesh);


	m_meshVertices.clear();

	for (int i = 0; i < mesh.nrOfVertices; i++)
	{
		m_readFile.read((char*)&vertex, sizeof(MyFileFormat::VertexData));

		vertex.positions.z = -vertex.positions.z;
		vertex.normals.z = -vertex.normals.z;
		vertex.uvs.y = -vertex.uvs.y;

		m_meshVertices.push_back(vertex);
	}

	m_meshVerticesSets.push_back(m_meshVertices);

	meshId++;
}

void MFF::ResetObject()
{
	m_meshHeader.clear();
	m_meshVertices.clear();
	m_meshVerticesSets.clear();
	meshId = 0;
}

MFF::MFF()
{

}

MFF::~MFF()
{

}

void MFF::SetFilePath(const char* p_Path)
{
	m_readPath = p_Path;
}

bool MFF::ReadFile()
{
	ResetObject();
	m_readFile.open(m_readPath, std::ios::binary | std::ios::in);

	if (m_readFile.is_open())
	{
		while (m_readFile.eof() == false)
		{
			MyFileFormat::SectionHeader section = MyFileFormat::SectionType::UNKNOWN;
			m_readFile.read((char*)&section, sizeof(MyFileFormat::SectionHeader));

			if (section.Type == MyFileFormat::SectionType::FILE_PATH)
			{
				ReadFilePath();
			}
			if (section.Type == MyFileFormat::SectionType::SCENE)
			{
				ReadScene();
			}
			if (section.Type == MyFileFormat::SectionType::MESH)
			{
				ReadMesh();
			}
		}
		m_readFile.close();
		return true;
	}
	else
		return false;
}

MFF& MFF::GetMFF()
{
	static MFF instance;
	return instance;
}

const MyFileFormat::Mesh& MFF::GetModel(unsigned int modelId)
{
	return m_meshHeader[modelId];
}

const int& MFF::GetNumberOfMeshesInScene()
{
	return m_scene.numberOfMeshes;
}

const std::vector<MyFileFormat::VertexData>& MFF::GetVertexSet(unsigned int modelId)
{
	return m_meshVerticesSets[modelId];
}
