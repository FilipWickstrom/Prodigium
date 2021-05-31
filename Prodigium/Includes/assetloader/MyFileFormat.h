#pragma once
const int MATERIAL_NAME_MAX_LENGTH = 50;
const int SHADING_MODEL_NAME_MAX_LENGTH = 50;
const int MESH_NAME_MAX_LENGTH = 50;

struct float4
{
	float x, y, z, w;

	float4(float X, float Y, float Z, float W) 
	{
		x = X; 
		y = Y; 
		z = Z; 
		w = W; 
	}

	float4() 
	{
		x = 0; 
		y = 0; 
		z = 0; 
		w = 0; 
	}
};
struct float3
{
	float x, y, z;

	float3(float X, float Y, float Z) 
	{ 
		x = X;
		y = Y; 
		z = Z; 
	}
	float3() 
	{
		x = 0;
		y = 0; 
		z = 0;
	}
};
struct float2
{
	float u, v;
	float2(float U, float V) 
	{ 
		u = U; 
		v = V;
	}
	float2() 
	{ 
		u = 0; 
		v = 0; 
	}
};

enum class SectionType
{
	UNKNOWN,
	FILE_INFO,
	SCENE,
	MESH,
	MATERIAL,
};

struct SectionHeader
{
	SectionType Type = SectionType::UNKNOWN;

	SectionHeader(SectionType _type)
	{
		Type = _type;
	}
	SectionHeader()
	{
		Type = SectionType::UNKNOWN;
	}
};

struct Scene : SectionHeader
{
	int nrOfMeshes = 0;

	Scene() : SectionHeader(SectionType::SCENE) {}
};

struct Mesh : SectionHeader
{
	int nrOfVertices = 0;
	int nrOfIndices = 0;
	char Name[MESH_NAME_MAX_LENGTH] = "";

	Mesh() : SectionHeader(SectionType::MESH) {}
};

struct VertexData
{
	float4 position;
	float3 normals;
	float2 textureCoords;
	float3 tangents;
	float3 biTangents;
};

struct Material : SectionHeader
{
	char Name[MATERIAL_NAME_MAX_LENGTH] = "";
	char ShadingModel[SHADING_MODEL_NAME_MAX_LENGTH] = "";

	float3 ambient = {};
	float3 diffuse = {};
	float3 specular = {};
	float opacity = 0;
	float shininess = 0;
	float reflectivity = 0;

	Material() : SectionHeader(SectionType::MATERIAL) {}
};