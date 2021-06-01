#pragma once

namespace MyFileFormat
{
#define MAX_FILE_PATH_NAME 256
#define MAX_NAME 64
	//Structs for float3 and float2 to make it cleaner than using float arrays
	struct float3
	{
		float X, Y, Z;

		float3(float x, float y, float z)
		{
			X = x;
			Y = y;
			Z = z;
		}
		float3()
		{
			X = 0.0f;
			Y = 0.0f;
			Z = 0.0f;
		}
	};

	struct float2
	{
		float X, Y;

		float2(float x, float y)
		{
			X = x;
			Y = y;
		}
		float2()
		{
			X = 0.0f;
			Y = 0.0f;
		}
	};

	enum class SectionType
	{
		UNKNOWN,
		FILE_PATH,
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

	struct FilePath : SectionHeader
	{
		char filePath[MAX_FILE_PATH_NAME] = {};

		FilePath() : SectionHeader(SectionType::FILE_PATH) {}
	};

	struct Scene : SectionHeader
	{
		int numberOfMeshes = 0;

		Scene() : SectionHeader(SectionType::SCENE) {}
	};

	struct Mesh : SectionHeader
	{
		char meshName[MAX_NAME] = {};
		int nrOfVertices = 0;

		Mesh() : SectionHeader(SectionType::MESH) {}
	};

	struct VertexData
	{
		float3 positions;
		float2 uvs;

		float3 normals;
		float3 tangent;
		float3 biTangent;
	};

	struct Material : SectionHeader
	{
		char materialName[MAX_NAME] = {};

		float3 ambientCol;
		float3 diffuseCol;
		float3 emissiveCol;
		float3 specularCol;

		float shininess = 0;

		short hasTexture = 0;
		char textureName[MAX_NAME] = {};
		char textureFilePath[MAX_FILE_PATH_NAME] = {};

		short hasNormalMap = 0;
		char normalMapName[MAX_NAME] = {};
		char normalMapFilePath[MAX_FILE_PATH_NAME] = {};

		Material() : SectionHeader(SectionType::MATERIAL) {}
	};	
}