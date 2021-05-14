#pragma once
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

#include "UsefulStructuresHeader.h"

class Animation
{
private:
	UINT nrOfBones;
	double currentFrame;
	double duraction;				//20
	double ticksPerSecond;			//24
	//theFrame = currentFrame + dt * ticksPerSecond is how many frame to go?

	struct Translation
	{
		//Times
		std::vector<double> positionTime = {};		//ADD OR NOT???
		std::vector<double> rotationTime = {};		//ADD OR NOT???
		std::vector<double> scaleTime = {};			//ADD OR NOT???
		//Actual values
		std::vector<aiVector3D> position = {};
		std::vector<aiVector3D> scale = {};
		std::vector<aiQuaternion> rotation = {};
	};
	//Translation lastTranslation;	//Will be used to interpolate later?

	//Every bone has it own positions, scale and rotation on each frame
	std::unordered_map<std::string, Translation> translations;

	//InterpolatePosition(time)
	//InterpolateRotation(time)
	//InterpolateScale(time)
public:
	Animation();
	~Animation();

	bool Load(std::string filename, std::unordered_map<std::string, UINT> boneMap);

	bool GetAnimationMatrix(std::string boneName, DirectX::SimpleMath::Matrix& animation);


	//Not the best... Sending copies of the matrices...?
	const std::vector<DirectX::SimpleMath::Matrix> GetAnimationMatrices(std::vector<std::string> allBones, double time);

	//GetModelMatrices(time, std::string boneName)
};