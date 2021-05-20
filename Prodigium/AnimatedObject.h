#pragma once
#include <fstream>
#include <unordered_map>

//#include "GameObject.h"
#include "ResourceManager.h"
#include "Animation.h"

const UINT MAXBONES = 25;
const UINT MAXTEXTURES = 2;

/*
Load in a model with bones, ".fbx" supported for now
Move the bones with different animations

If optimalization is needed:
	- Use only 4 weights per vertex
	- Lower total number of bones
*/

/*
TODO:
	- When adding a animated object with name: "Player.fbx"
	- We send in "Player" to animation
	- We look for animations named: Player_Idle, Player_Walking, Player_Running
*/

//Switch between states of animations //ADD LEFT AND RIGHT STRAFE?
enum class AnimationState
{
	IDLE, IDLE2, WALKFORWARD, WALKBACKWARD, RUNFORWARD, RUNBACKWARD, NONE, NROFSTATE = 7
};

class AnimatedObject
{
private:
	//Needs a seperate vertexshader and input for it
	ID3D11VertexShader* vertexShader;
	std::string vShaderByteCode;
	ID3D11InputLayout* inputlayout;

	//Rendering the mesh
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	UINT indexCount;

	//Holds each bones final matrix
	ID3D11Buffer* boneMatricesBuffer;

	//Tree structure of bones instead of saving assimps larger version
	struct Bone
	{
		UINT id = 0;
		std::string name = "";
		//Bonespace to mesh-space in bindpose (T-pose)
		DirectX::SimpleMath::Matrix inverseBind = {};
		std::vector<Bone> children = {};
	} rootBone;

	//Name of the bone refers to which ID it has
	std::unordered_map<std::string, UINT> boneMap;
	std::vector<std::string> boneNames;
	
	//Updated every frame
	std::vector<DirectX::SimpleMath::Matrix> animatedMatrices;	//Matrices that will be calculated from saved animation information
	std::vector<DirectX::SimpleMath::Matrix> modelMatrices;		//
	std::vector<DirectX::SimpleMath::Matrix> finalMatrices;		//Final matrices that the GPU will use

	AnimationState currentState;
	
	//FIX AND CLEANUP THIS!!!****
	Animation* currentAnim;
	std::vector<Animation*> allAnimations;


	//Animation* walkRunAnim;
	//Animation* idleAnim;

public:
	//All the positions of the T-posing character
	std::vector<DirectX::SimpleMath::Vector3> meshPositions;

private:
	//Basic parts to be able to render vertices
	bool LoadVertexShader();
	bool CreateInputLayout();
	bool CreateVertIndBuffers(const std::vector<AnimationVertex>& vertices, 
							  const std::vector<UINT>& indices, UINT nrOfIndices);
	
	//Recursive function to make a bone tree from ASSIMP's nodes. Depth first search
	bool LoadBoneTree(Bone& currentBone, 
					  aiNode* node, 
					  std::unordered_map<std::string,std::pair<UINT, DirectX::SimpleMath::Matrix>>& tempMap);
	
	//Load in a mesh with a skeleton
	bool LoadRiggedMesh(std::string animFolder);
	bool LoadAnimations(std::string animFolder);

	bool CreateBonesCBuffer();
	void UpdateBonesCBuffer();
	
	void CalcFinalMatrix(Bone& currentBone, UINT parentID, const DirectX::SimpleMath::Matrix& worldMatrix);

public:
	AnimatedObject();
	virtual ~AnimatedObject();

	bool Initialize(std::string animFolder);					
	
	void ChangeAnimState(AnimationState state);

	//With animate set to false, we can render without changing pose.
	//Can be used when rendering shadows
	void Render(const DirectX::SimpleMath::Matrix& worldMatrix, bool animate = true);

};
