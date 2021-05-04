#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "GameObject.h"

/*
Load in a model with bones, .fbx-format supported for now
Move the bones with different animations
*/

class AnimatedObject : public GameObject
{
private:
	//game objects modelMatrix is roots matrix
	
	//The model mesh
	//Bones for the model

	//Vector of animations
		//animation

	//std::vector<Mesh> allMeshes;
	//std::vector<Bone> allBones;
	//		bone struct
	//			name
	//			vertexweight
	//			offsetmatrix
	// 
	//std::vector<vertex> vertices;

	//current frame
	//max frames



	/*
	enum allAnimations:
		idle
		walk
		run
	*/

private:
	//Load mesh and bones
	bool LoadAnimation(std::string animationFile);	//Later also vector of animations


public:
	AnimatedObject();
	virtual ~AnimatedObject();

	bool Initialize();
		//1. Load in the mesh with all the bones
		//2. Load in the animations
		//3.

	
	//ChangeAnimation(allAnimations : animation)

	//void Render(with deltatime?)

};
