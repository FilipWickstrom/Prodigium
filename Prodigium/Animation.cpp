#include "Animation.h"
using namespace DirectX::SimpleMath;

std::pair<UINT, UINT> Animation::FindTwoKeyframes(const std::string& boneName, const std::vector<double>& keyFrames)
{
	UINT closestLeft = 0;
	UINT closestRight = 1;
	bool found = false;

	//Searching through all keyframes to find where current time is between
	//[EXTRA]: Could be optimized with saving previously used keyframes
	for (UINT i = 1; i < (UINT)keyFrames.size() && !found; i++)
	{
		UINT left = i;
		UINT right = (i + 1) % (UINT)keyFrames.size();

		//Current time has to be in between the two keyframes
		if (this->currentFrameTime > keyFrames[left] &&
			this->currentFrameTime < keyFrames[right])
		{
			closestLeft = left;
			closestRight = right;
			found = true;
		}
	}
	return std::pair<UINT, UINT>(closestLeft, closestRight);
}

aiVector3D Animation::InterpolatePosition(const std::string& boneName)
{
	//Get the closest keyframes
	std::pair<UINT, UINT> frames = FindTwoKeyframes(boneName, this->translations[boneName].positionTime);
	UINT firstFrame = frames.first;
	UINT secondFrame = frames.second;

	//Calculates a floatvalue on how close the closestFrame is compared to nextFrame
	double firstTime = this->translations[boneName].positionTime[firstFrame];
	double secondTime = this->translations[boneName].positionTime[secondFrame];
	//Gets a value between 0.0f to 1.0f of how much to interpolate
	float lerpTime = float( (this->currentFrameTime - firstTime) / (secondTime - firstTime));

	//Need to convert to simplemath as ASSIMP does not have any lerp-functions
	aiVector3D aiFirst = this->translations[boneName].position[firstFrame];
	Vector3 firstPos = Vector3(aiFirst.x, aiFirst.y, aiFirst.z);
	aiVector3D aiSecond = this->translations[boneName].position[secondFrame];
	Vector3 secondPos = Vector3(aiSecond.x, aiSecond.y, aiSecond.z);
	
	//Finally lerp to get a position that is in between this two positions
	Vector3 lerped = Vector3::Lerp(firstPos, secondPos, lerpTime);
	return aiVector3D(lerped.x, lerped.y, lerped.z);
}

aiVector3D Animation::InterpolateScale(const std::string& boneName)
{
	std::pair<UINT, UINT> frames = FindTwoKeyframes(boneName, this->translations[boneName].scaleTime);
	UINT firstFrame = frames.first;
	UINT secondFrame = frames.second;

	double firstTime = this->translations[boneName].scaleTime[firstFrame];
	double secondTime = this->translations[boneName].scaleTime[secondFrame];
	float lerpTime = float((this->currentFrameTime - firstTime) / (secondTime - firstTime));

	aiVector3D aiFirst = this->translations[boneName].scale[firstFrame];
	Vector3 firstScl = Vector3(aiFirst.x, aiFirst.y, aiFirst.z);
	aiVector3D aiSecond = this->translations[boneName].scale[secondFrame];
	Vector3 secondScl = Vector3(aiSecond.x, aiSecond.y, aiSecond.z);
	
	Vector3 lerped = Vector3::Lerp(firstScl, secondScl, lerpTime);
	return aiVector3D(lerped.x, lerped.y, lerped.z);
}

aiQuaternion Animation::InterpolateRotation(const std::string& boneName)
{
	std::pair<UINT, UINT> frames = FindTwoKeyframes(boneName, this->translations[boneName].rotationTime);
	UINT firstFrame = frames.first;
	UINT secondFrame = frames.second;

	double firstTime = this->translations[boneName].rotationTime[firstFrame];
	double secondTime = this->translations[boneName].rotationTime[secondFrame];
	float lerpTime = float((this->currentFrameTime - firstTime) / (secondTime - firstTime));

	aiQuaternion aiFirst = this->translations[boneName].rotation[firstFrame];
	Quaternion firstRot = Quaternion(aiFirst.x, aiFirst.y, aiFirst.z, aiFirst.w);
	aiQuaternion aiSecond = this->translations[boneName].rotation[secondFrame];
	Quaternion secondRot = Quaternion(aiSecond.x, aiSecond.y, aiSecond.z, aiSecond.w);

	//Need to use "Spherical linear interpolation" instead of regular "lerp"
	Quaternion lerped = Quaternion::Slerp(firstRot, secondRot, lerpTime);
	lerped.Normalize();

	//The order is a bit different between ASSIMP and Simplemath
	return aiQuaternion(lerped.w, lerped.x, lerped.y, lerped.z);
}

Animation::Animation()
{
	this->nrOfBones = 0;
	this->currentFrameTime = 0;
	this->duration = 0;
	this->ticksPerSecond = 0;
	this->loopable = true;
	this->reachedEnd = false;
}

Animation::~Animation()
{
	for (auto& item : this->translations)
	{
		item.second.position.clear();
		item.second.positionTime.clear();
		item.second.rotation.clear();
		item.second.rotationTime.clear();
		item.second.scale.clear();
		item.second.scaleTime.clear();
	}
	this->translations.clear();
}

bool Animation::Load(std::string filename, std::unordered_map<std::string, UINT> boneMap, bool looping, int animSpeed)
{
	Assimp::Importer importer;
	
	//Will remove extra text on bones like: "_$AssimpFbx$_"...
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
	const aiScene* scene = importer.ReadFile("Models/" + filename,
											aiProcess_FlipWindingOrder |	//Make clockwise order
											aiProcess_MakeLeftHanded);

	//The file either was not possible to open or does not simply exists
	if (!scene || !scene->mRootNode)
	{
		//std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
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
	this->duration = animation->mDuration;
	
	//Use default by assimp
	if (animSpeed == 0)
		this->ticksPerSecond = animation->mTicksPerSecond;
	else
		this->ticksPerSecond = (double)animSpeed;

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
		std::cout << "Animation: '" << filename << "' has been loaded" << std::endl;
	#endif

	this->loopable = looping;
	importer.FreeScene();
	return true;
}

void Animation::GetAnimationMatrices(const std::vector<std::string>& allBones, 
									 std::vector<DirectX::SimpleMath::Matrix>& animMatrices, 
									 bool interpolate)
{	
	//Keep going as long as we have not reached end of a non-looping animation
	if (!this->reachedEnd)
	{
		//Adding to the current frame
		this->currentFrameTime += this->ticksPerSecond * Graphics::GetDeltaTime();

		//Resets to start when reached end
		if (this->currentFrameTime >= this->duration)
		{
			if (!this->loopable)
			{
				this->reachedEnd = true;
				this->currentFrameTime -= this->ticksPerSecond * Graphics::GetDeltaTime();
			}
			else
			{
				this->currentFrameTime = 0;
			}
		}

		//Resets to end when reached start - only when playing in revers
		else if (this->currentFrameTime < 0)
		{
			if (!this->loopable)
			{
				this->reachedEnd = true;
				this->currentFrameTime -= this->ticksPerSecond * Graphics::GetDeltaTime();
			}
			else
			{ 
				this->currentFrameTime = this->duration + this->currentFrameTime;
			}
		}

		aiVector3D pos;
		aiVector3D scl;
		aiQuaternion rot;

		for (UINT i = 0; i < allBones.size(); i++)
		{
			Matrix animation = Matrix::Identity;
			std::string boneName = allBones[i];
			if (this->translations.find(boneName) != this->translations.end())
			{
				if (interpolate)
				{
					pos = InterpolatePosition(boneName);
					scl = InterpolateScale(boneName);
					rot = InterpolateRotation(boneName);
				}
				else
				{
					UINT closestPos = FindTwoKeyframes(boneName, this->translations[boneName].positionTime).first;
					UINT closestScl = FindTwoKeyframes(boneName, this->translations[boneName].scaleTime).first;
					UINT closestRot = FindTwoKeyframes(boneName, this->translations[boneName].rotationTime).first;
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
}

void Animation::SetAnimationSpeed(int animSpeed)
{
	//Animationspeed can't be 0
	if (animSpeed != 0)
	{
		this->ticksPerSecond = (double)animSpeed;
	}
}

void Animation::ResetCurrentTime()
{
	this->currentFrameTime = 0;
}

void Animation::ResetReachedEnd()
{
	if (this->reachedEnd)
		this->reachedEnd = false;
}

bool Animation::HasReachedEnd()
{
	return this->reachedEnd;
}
