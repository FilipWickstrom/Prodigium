#include "Assetloader.h"
#include <fstream>

Assetloader::Assetloader()
{
    this->mesh = {};
    this->vertices = {};
    this->mat = {};
    this->indices = {};
    this->defaultPath = "../Models/";
}

Assetloader::~Assetloader()
{

}

const bool Assetloader::LoadModel(const std::string& fileName) const
{
    std::string fullFilePath = this->defaultPath + fileName;
    std::ifstream input(fullFilePath.c_str(), std::ifstream::binary);     
    
    if (input)
    {
        input.read((char*)&this->scene, sizeof(Scene));
        int vertexIndex = 0;
        int indicesIndex = 0;

        for (int i = 0; i < this->scene.nrOfMeshes; i++)
        {
            input.read((char*)&this->mesh[i], sizeof(Mesh));
            
            for (vertexIndex; vertexIndex < this->mesh[i].nrOfVertices; vertexIndex++)
            {
                input.read((char*)&this->vertices[vertexIndex], sizeof(VertexData));
            }
            for (indicesIndex; indicesIndex < this->mesh[i].nrOfIndices; indicesIndex++)
            {
                input.read((char*)&this->indices[indicesIndex], sizeof(int));
            }
        }

        input.read((char*)&this->mat, sizeof(Material));

        input.close();

        return true;
    }
    else
        return false;
}

void Assetloader::SetDefaultPath(const std::string& newDefaultPath)
{
    this->defaultPath = newDefaultPath;
}

const std::vector<VertexData>& Assetloader::GetVertexData() const
{
    return this->vertices;
}

const std::vector<int>& Assetloader::GetIndices() const
{
    return this->indices;
}

const Material& Assetloader::GetMaterial() const
{
    return this->mat;
}
