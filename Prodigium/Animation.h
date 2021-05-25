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
	double currentFrameTime;
	double duration;				//How many frames the animation is
	double ticksPerSecond;			//Framerate of the animation
	
	//Special for animations that should not loop
	bool loopable;
	bool reachedEnd;

	struct Translation
	{
		//Times of when something change
		std::vector<double> positionTime = {};
		std::vector<double> rotationTime = {};
		std::vector<double> scaleTime = {};
		//Actual values of the change
		std::vector<aiVector3D> position = {};
		std::vector<aiVector3D> scale = {};
		std::vector<aiQuaternion> rotation = {};
	};

	//Every bone has it own positions, scale and rotation on each frame
	std::unordered_map<std::string, Translation> translations;

	//Finds the two keyframes that current time is in between
	std::pair<UINT, UINT> FindTwoKeyframes(const std::string& boneName, const std::vector<double>& keyFrames);

	//Interpolating between two positions, scales or rotations
	aiVector3D InterpolatePosition(const std::string& boneName);
	aiVector3D InterpolateScale(const std::string& boneName);
	aiQuaternion InterpolateRotation(const std::string& boneName);

public:
	Animation();
	~Animation();

	//Load in the animation. Only takes the bones that was loaded from the object
	bool Load(std::string filename, std::unordered_map<std::string, UINT> boneMap, bool looping = true, int animSpeed = 0);

	//Gets the animation matrices at this time
	void GetAnimationMatrices(const std::vector<std::string>& allBones, 
							 std::vector<DirectX::SimpleMath::Matrix>& animMatrices, 
							 bool interpolate = true);

	void SetAnimationSpeed(int animSpeed);
	void ResetCurrentTime();
	void ResetReachedEnd();
	bool HasReachedEnd();
};