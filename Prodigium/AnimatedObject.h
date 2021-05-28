#pragma once
#include <fstream>
#include <unordered_map>
#include "Animation.h"
#include "GUIHandler.h"
#include "Resource.h"

const UINT MAXBONES = 25;

/*
Load in a model with bones, ".fbx" supported for now
Move the bones with different animations

If optimalization is needed:
	- Use only 3 weights per vertex
	- Lower total number of bones
	- Fix search closest two keyframes in "Animation.cpp"
*/

//Switch between states of animations
enum class AnimationState
{
	NONE, 
	IDLE, IDLE2, 
	WALKFORWARD, WALKBACKWARD, 
	RUNFORWARD, RUNBACKWARD, 
	WALKLEFT, RUNLEFT,
	WALKRIGHT, RUNRIGHT, 
	DEAD, PICKUP
};

class AnimatedObject : public Resource 
{
private:
	//Needs a seperate vertexshader and input for it
	ID3D11VertexShader* vertexShader;
	std::string vShaderByteCode;
	ID3D11InputLayout* inputlayout;

	//Shadow specific settings
	ID3D11VertexShader* shadowVertexShader;
	ID3D11InputLayout* shadowInputlayout;
	std::string shadowVShaderByteCode;

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
	
	//Handles which state we are on and what animation to use
	AnimationState currentState;

	std::vector<Animation*>animationList;
	std::unordered_map<AnimationState, UINT> animStates;

	//This setting will make the animation
	//smoother but will cost some extra frames
	bool useInterpolation;

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
	void LoadAnimations(std::string animFolder);

	bool CreateBonesCBuffer();
	void UpdateBonesCBuffer();
	
	void CalcFinalMatrix(Bone& currentBone, UINT parentID, const DirectX::SimpleMath::Matrix& worldMatrix);
	
public:
	AnimatedObject();
	virtual ~AnimatedObject();

	bool Initialize(std::string animFolder);					
	
	void ChangeAnimState(AnimationState state);
	void UseInterpolation(bool toggle = true);
	bool AnimationReachedEnd();
	const AnimationState& GetAnimationState();

	//With animate set to false, we can render without changing pose.
	//Can be used when rendering shadows
	void Render(const DirectX::SimpleMath::Matrix& worldMatrix, bool animate = true);
	void RenderShadows(const DirectX::SimpleMath::Matrix& worldMatrix);
};
