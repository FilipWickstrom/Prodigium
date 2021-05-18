#include "Animation.h"
using namespace DirectX::SimpleMath;

UINT Animation::GetClosestKeyframe(const std::string& boneName, const std::vector<double>& keyFrameTimes)
{
	//Find the closest keyframe to current time
	UINT closest = 0;

	//Search though all the keyframes to find the closest to currentFrameTime
	for (size_t i = 1; i < keyFrameTimes.size(); i++)
	{
		if (abs(this->currentFrameTime - keyFrameTimes[i]) < abs(this->currentFrameTime - keyFrameTimes[closest]))
		{
			closest = UINT(i);
		}
	}
	return closest;
}

aiVector3D Animation::InterpolatePosition(const std::string& boneName)
{
	//Get the closest keyframes
	UINT closestFrame = GetClosestKeyframe(boneName, this->translations[boneName].positionTime);
	UINT nextFrame = (closestFrame + 1) % (UINT)(this->translations[boneName].positionTime.size());

	//Calculates a floatvalue on how close the closestFrame is compared to nextFrame
	double closestTime = this->translations[boneName].positionTime[closestFrame];
	double nextTime = this->translations[boneName].positionTime[nextFrame];
	float lerpTime = float(abs(this->currentFrameTime - closestTime) / (abs(nextTime - closestTime)));

	//Need to convert to simplemath as ASSIMP does not have any lerp-functions
	aiVector3D aiClosest = this->translations[boneName].position[closestFrame];
	Vector3 closestPos = Vector3(aiClosest.x, aiClosest.y, aiClosest.z);
	aiVector3D aiNext = this->translations[boneName].position[nextFrame];
	Vector3 nextPos = Vector3(aiNext.x, aiNext.y, aiNext.z);
	
	//Finally lerp to get a position that is in between this two positions
	Vector3 lerped = Vector3::Lerp(closestPos, nextPos, lerpTime);
	return aiVector3D(lerped.x, lerped.y, lerped.z);
}

aiVector3D Animation::InterpolateScale(const std::string& boneName)
{
	UINT closestFrame = GetClosestKeyframe(boneName, this->translations[boneName].scaleTime);
	UINT nextFrame = (closestFrame + 1) % (UINT)(this->translations[boneName].scaleTime.size());

	double closestTime = this->translations[boneName].scaleTime[closestFrame];
	double nextTime = this->translations[boneName].scaleTime[nextFrame];
	float lerpTime = float(abs(this->currentFrameTime - closestTime) / (abs(nextTime - closestTime)));

	aiVector3D aiClosest = this->translations[boneName].scale[closestFrame];
	Vector3 closestScl = Vector3(aiClosest.x, aiClosest.y, aiClosest.z);
	aiVector3D aiNext = this->translations[boneName].scale[nextFrame];
	Vector3 nextScl = Vector3(aiNext.x, aiNext.y, aiNext.z);
	
	Vector3 lerped = Vector3::Lerp(closestScl, nextScl, lerpTime);
	return aiVector3D(lerped.x, lerped.y, lerped.z);
}

aiQuaternion Animation::InterpolateRotation(const std::string& boneName)
{
	UINT closestFrame = GetClosestKeyframe(boneName, this->translations[boneName].rotationTime);
	UINT nextFrame = (closestFrame + 1) % (UINT)(this->translations[boneName].rotationTime.size());

	double closestTime = this->translations[boneName].rotationTime[closestFrame];
	double nextTime = this->translations[boneName].rotationTime[nextFrame];
	float lerpTime = float(abs(this->currentFrameTime - closestTime) / (abs(nextTime - closestTime)));

	aiQuaternion aiClosest = this->translations[boneName].rotation[closestFrame];
	Quaternion closestRot = Quaternion(aiClosest.x, aiClosest.y, aiClosest.z, aiClosest.w);
	aiQuaternion aiNext = this->translations[boneName].rotation[nextFrame];
	Quaternion nextRot = Quaternion(aiNext.x, aiNext.y, aiNext.z, aiNext.w);

	//Need to use "Spherical linear interpolation" instead of regular "lerp"
	Quaternion lerped = Quaternion::Slerp(closestRot, nextRot, lerpTime);

	//The order is a bit different between ASSIMP and Simplemath
	return aiQuaternion(lerped.w, lerped.x, lerped.y, lerped.z).Normalize();
}

Animation::Animation()
{
	this->nrOfBones = 0;
	this->currentFrameTime = 0;
	this->duraction = 0;
	this->ticksPerSecond = 0;
	this->useInterpolation = true;
}

Animation::~Animation()
{
}

bool Animation::Load(std::string filename, std::unordered_map<std::string, UINT> boneMap, UINT animSpeed)
{
	Assimp::Importer importer;
	
	//Will remove extra text on bones like: "_$AssimpFbx$_"...
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene* scene = importer.ReadFile("Models/" + filename,
											aiProcess_FlipWindingOrder |	//Make clockwise order
											aiProcess_MakeLeftHanded);

	if (!scene || !scene->mRootNode)
	{
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		importer.FreeScene();
		return false;
	}

	if (!scene->HasAnimations())
	{
		std::cout << "The file " << filename << " does not have any animations..." << std::endl;
		importer.FreeScene();
		return false;
	}

	//Takes only one animation
	const aiAnimation* animation = scene->mAnimations[0];

	//Set the values for this type of animation
	this->duraction = animation->mDuration;
	
	//Use default by assimp
	if (animSpeed == 0)
		this->ticksPerSecond = animation->mTicksPerSecond;
	else
		this->ticksPerSecond = animSpeed;

	//Go through all the channels with animations
	for (unsigned int i = 0; i < animation->mNumChannels; i++)
	{
		const aiNodeAnim* channel = animation->mChannels[i];
		std::string boneName = channel->mNodeName.data;
		
		//Remove this part of the string as it will fail searching in bones later...
		std::string removeText = "_$AssimpFbx$_";
		size_t index = boneName.find(removeText);
		if (index != std::string::npos)
		{
			boneName.erase(index);
		}
		
		//Check if the bone exists in the map
		if (boneMap.find(boneName) != boneMap.end())
		{
			Translation bonesTranslations;
			
			bonesTranslations.position.reserve(size_t(channel->mNumPositionKeys));
			bonesTranslations.positionTime.reserve(size_t(channel->mNumPositionKeys));
			for (UINT pos = 0; pos < (UINT)channel->mNumPositionKeys; pos++)
			{
				bonesTranslations.positionTime.push_back(channel->mPositionKeys[pos].mTime);
				bonesTranslations.position.push_back(channel->mPositionKeys[pos].mValue);
			}

			bonesTranslations.rotation.reserve(size_t(channel->mNumRotationKeys));
			bonesTranslations.rotationTime.reserve(size_t(channel->mNumRotationKeys));
			for (UINT rot = 0; rot < (UINT)channel->mNumRotationKeys; rot++)
			{
				bonesTranslations.rotationTime.push_back(channel->mRotationKeys[rot].mTime);
				bonesTranslations.rotation.push_back(channel->mRotationKeys[rot].mValue);
			}

			bonesTranslations.scale.reserve(size_t(channel->mNumScalingKeys));
			bonesTranslations.scaleTime.reserve(size_t(channel->mNumScalingKeys));
			for (UINT scl = 0; scl < (UINT)channel->mNumScalingKeys; scl++)
			{
				bonesTranslations.scaleTime.push_back(channel->mScalingKeys[scl].mTime);
				bonesTranslations.scale.push_back(channel->mScalingKeys[scl].mValue);
			}

			this->translations[boneName] = bonesTranslations;
			this->nrOfBones++;
		}
	}

	#ifdef _DEBUG
		//Writing out some stats
		std::cout << "Animation: '" << filename << "' has " << this->nrOfBones << " bones" << std::endl;
		std::cout << "Duration: " << this->duraction << std::endl;
		std::cout << "Ticks per second: " << this->ticksPerSecond << std::endl;
	#endif

	importer.FreeScene();
	return true;
}

void Animation::GetAnimationMatrices(const std::vector<std::string>& allBones, std::vector<DirectX::SimpleMath::Matrix>& animMatrices)
{	
	//Adding to the current frame
	this->currentFrameTime += this->ticksPerSecond * Graphics::GetDeltaTime();
	
	//Reset when reached end
	if (this->currentFrameTime >= this->duraction)
		this->currentFrameTime = 0;

	aiVector3D pos;
	aiVector3D scl;
	aiQuaternion rot;

	for (UINT i = 0; i < allBones.size(); i++)
	{
		Matrix animation = Matrix::Identity;
		std::string boneName = allBones[i];
		if (this->translations.find(boneName) != this->translations.end())
		{
			if (this->useInterpolation)
			{
				pos = InterpolatePosition(boneName);
				scl = InterpolateScale(boneName);
				rot = InterpolateRotation(boneName);
			}
			else
			{
				UINT closestPos = GetClosestKeyframe(boneName, this->translations[boneName].positionTime);
				UINT closestScl = GetClosestKeyframe(boneName, this->translations[boneName].scaleTime);
				UINT closestRot = GetClosestKeyframe(boneName, this->translations[boneName].rotationTime);
				pos = this->translations[boneName].position[closestPos];
				scl = this->translations[boneName].scale[closestScl];
				rot = this->translations[boneName].rotation[closestRot];
			}

			aiMatrix4x4 aiMat = aiMatrix4x4(scl, rot, pos);
			animation = Matrix(&aiMat.a1);
		}
		animMatrices[i] = animation;
	}
}

void Animation::SetAnimationSpeed(UINT animSpeed)
{
	//Animationspeed can't be 0
	if (animSpeed != 0)
	{
		this->ticksPerSecond = animSpeed;
	}
}
