#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <fstream>
#include <unordered_map>

#include "GameObject.h"
#include "UsefulStructuresHeader.h"
#include "ResourceManager.h"

const UINT MAXBONES = 25;
const UINT MAXTEXTURES = 2;

/*
Load in a model with bones, .fbx-format supported for now
Move the bones with different animations
*/

class AnimatedObject : public GameObject
{
private:
	//Needs a seperate vertexshader and input to it
	ID3D11VertexShader* vertexShader;
	std::string vShaderByteCode;
	ID3D11InputLayout* inputlayout;

	//Rendering the mesh
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT indexCount;
	ID3D11ShaderResourceView* textureSRVs[MAXTEXTURES];


	ID3D11Buffer* boneMatricesBuffer;

	//Tree structure of bones instead of saving assimps larger version		- MOVE TO USEFUL STRUCTURES?
	struct Bone
	{
		UINT id = -1;
		std::string name = "";
		//Bonespace to mesh-space in bindpose (T-pose) - Inverse bind matrix
		DirectX::SimpleMath::Matrix bindMatrix;
		std::vector<Bone> children = {};
	} rootBone;	//Hips in this case

	//Name of the bone refers to which ID it has. Can be used to search up in vectors later?
	std::unordered_map<std::string, UINT> boneMap;
	std::vector<DirectX::SimpleMath::Matrix> invBindMatrix;

	std::vector<DirectX::SimpleMath::Matrix> finalMatrices;

	
	//Bones
	//std::vector<UINT>parentArray;
	//std::vector<DirectX::SimpleMath::Matrix> inverseBindPoses;	//Does not change - how they are relative to each other from start
	//std::vector<DirectX::SimpleMath::Matrix> modelMatrices;		//Changes dynamic - current model transforms
	//std::vector<DirectX::SimpleMath::Matrix> localMatrices;		//Changes dynamic - current local transforms	//relative to parent
	//std::vector<DirectX::SimpleMath::Matrix> finalMatrices;		//Also dynamic - final version that gets uploaded to GPU. Need or not???

	//game objects modelMatrix is roots matrix
	//Vector of animations
		//animation

	//current frame
	//max frames

	/*enum allAnimations:
		idle
		walk
		run */

private:
	//Basic parts to be able to render vertices
	bool LoadVertexShader();
	bool CreateInputLayout();
	bool CreateVertIndBuffers(const std::vector<AnimationVertex>& vertices, 
							  const std::vector<unsigned short>& indices, UINT nrOfIndices);
	
	//Recursive function to make a bone tree from ASSIMP's nodes. Depth first search
	bool LoadBoneTree(Bone& currentBone, aiNode* node, std::unordered_map<std::string,
					std::pair<UINT, DirectX::SimpleMath::Matrix>>& tempMap);
	
	//Load in a mesh with a skeleton
	bool LoadRiggedMesh(std::string riggedModelFile);

	//Load in a animation
	bool LoadAnimation(std::string animationFile);

	bool CreateBonesCBuffer();
	bool LoadTextures(std::string diffuse, std::string normalMap = "");

	void SetupInvBindMatrices(Bone& currentBone, std::vector<DirectX::SimpleMath::Matrix>& invBindMatrices);
	void TestLoop(Bone& currentBone, Bone& parentBone, std::vector<DirectX::SimpleMath::Matrix>& matrices);

public:
	AnimatedObject();
	virtual ~AnimatedObject();

	bool Initialize(std::string tposeFile, std::string diffuse, std::string normalMap = "");
		//1. Load in the mesh with all the bones
		//2. Load in the animations
		//3.

	
	//ChangeAnimation(allAnimations : animation)

	void Render();		//Deltatime included


};
