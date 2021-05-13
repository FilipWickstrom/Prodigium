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
	double currentFrame;
	double duraction;				//Frames?	//299 frames
	double ticksPerSecond;			//30
	//theFrame = currentFrame + dt * ticksPerSecond is how many frame to go?

	struct Translation
	{
		std::vector<aiVector3D> position = {};
		std::vector<aiVector3D> scale = {};
		std::vector<aiQuaternion> rotation = {};
	};
	Translation lastTranslation;	//Will be used to interpolate later?

	//Every bone has it own positions, scale and rotation on each frame
	std::unordered_map<std::string, Translation> translations;

	//InterpolatePosition(time)
	//InterpolateRotation(time)
	//InterpolateScale(time)
public:
	Animation();
	~Animation();

	bool Load(std::string filename, std::unordered_map<std::string, UINT> boneMap);

	bool GetAnimationMatrix(unsigned int keyFrame, std::string boneName, DirectX::SimpleMath::Matrix& animation);

	//GetModelMatrices(time, std::string boneName)
};