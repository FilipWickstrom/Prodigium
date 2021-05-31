#pragma once
#include "MyFileFormat.h"
#include <vector>
#include <string>

class Assetloader
{
private:
	Scene scene;
	std::vector<Mesh> mesh;
	std::vector<VertexData> vertices;
	std::vector<int> indices;
	Material mat;
	std::string defaultPath = "../Models/";
public:
	Assetloader();
	~Assetloader();

	/*
	Loads the model from file.
	Default filepath is ../Models/
	Can be altered with SetDefaultPath()
	*/
	const bool LoadModel(const std::string& fileName) const;

	//Sets the Default filepath from "../Models/" to newDefaultPath
	void SetDefaultPath(const std::string& newDefaultPath);

	//Returns a vetor of the type VertexData, which includes the positions, uvs, normals, tangents and bitangents for every vertex in the scene
	const std::vector<VertexData>& GetVertexData() const;

	//Return a vector of the type int, which includes all the indices of the scene
	const std::vector<int>& GetIndices() const;

	//Returns the material of the model, which includes ambient color, diffuse color, specular color, opacity, shininess, reflectivity, name of the Material and name of the ShadingModel
	const Material& GetMaterial()const;
};

