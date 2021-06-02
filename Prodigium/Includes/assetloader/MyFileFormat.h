#pragma once

namespace MyFileFormat
{
#define MAX_FILE_PATH_NAME 256
#define MAX_NAME 64

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
		float x, y;

		float2(float X, float Y)
		{
			x = X;
			y = Y;
		}
		float2()
		{
			x = 0;
			y = 0;
		}
	};
	
	enum class SectionType
	{
		UNKNOWN,
		FILE_PATH,
		SCENE,
		MESH,
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
}