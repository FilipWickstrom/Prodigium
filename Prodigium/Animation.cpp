#include "Animation.h"
using namespace DirectX::SimpleMath;

Animation::Animation()
{
	this->nrOfBones = 0;
	this->currentFrame = 0;
	this->duraction = 0;
	this->ticksPerSecond = 0;
}

Animation::~Animation()
{
}

bool Animation::Load(std::string filename, std::unordered_map<std::string, UINT> boneMap)
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
	this->ticksPerSecond = animation->mTicksPerSecond;

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
			aiVector3D position = {};
			aiVector3D scale = {};
			aiQuaternion rotation = {};
			Translation bonesTranslations;

			//Go through all the frames
			for (unsigned int key = 0; key < (unsigned int)animation->mDuration; key++)
			{
				//Update with new information - otherwise use the previous data
				if (key < channel->mNumPositionKeys)
				{
					position = channel->mPositionKeys[key].mValue;
				}
				if (key < channel->mNumRotationKeys)
				{
					rotation = channel->mRotationKeys[key].mValue;
				}
				if (key < channel->mNumScalingKeys)
				{
					scale = channel->mScalingKeys[key].mValue;
				}

				bonesTranslations.position.push_back(position);
				bonesTranslations.rotation.push_back(rotation);
				bonesTranslations.scale.push_back(scale);
			}

			this->translations[boneName] = bonesTranslations;
			this->nrOfBones++;
		}
	}

	//Writing out some stats
	std::cout << "Loaded animation: " << filename << " with " <<  this->nrOfBones << " bones!" << std::endl;
	std::cout << "Duration: " << this->duraction << std::endl;
	std::cout << "Ticks per second: " << this->ticksPerSecond << std::endl;

	importer.FreeScene();
	return true;
}

bool Animation::GetAnimationMatrix(std::string boneName, Matrix& animation)
{
	//Returns true if we got an animation matrix on that bone.
	//Returns the matrix in paramater. 
	//All positions, scale and rotation is absolute in that space

	int keyFrame = 12;

	//See if the bone exists in the unordered map
	if (this->translations.find(boneName) != this->translations.end())
	{
		aiVector3D pos = this->translations[boneName].position[keyFrame];
		aiVector3D scl = this->translations[boneName].scale[keyFrame];
		aiQuaternion rot = this->translations[boneName].rotation[keyFrame].Normalize();
		aiMatrix4x4 aiMat = aiMatrix4x4(scl, rot, pos);
		animation = Matrix(&aiMat.a1);
		return true;
	}
	else
	{
		return false;
	}
}

const std::vector<DirectX::SimpleMath::Matrix> Animation::GetAnimationMatrices(std::vector<std::string> allBones, double time)
{
	std::vector<Matrix> animatedMatrices;
	
	//Adding to the current frame
	this->currentFrame += this->ticksPerSecond * Graphics::GetDeltaTime();

	//Reset when reached end
	if (this->currentFrame >= this->duraction)
		this->currentFrame = 0;

	UINT keyFrame = UINT(this->currentFrame);
	//UINT firstFrame;	//current
	//UINT secondFrame;	//current + 1?

	for (UINT i = 0; i < allBones.size(); i++)
	{
		Matrix animation = Matrix::Identity;
		std::string boneName = allBones[i];
		if (this->translations.find(boneName) != this->translations.end())
		{
			//Interpolate between two versions
			
			//Get first frame


			aiVector3D pos = this->translations[boneName].position[keyFrame];
			aiVector3D scl = this->translations[boneName].scale[keyFrame];
			aiQuaternion rot = this->translations[boneName].rotation[keyFrame].Normalize();
			aiMatrix4x4 aiMat = aiMatrix4x4(scl, rot, pos);
			animation = Matrix(&aiMat.a1);

		}
		animatedMatrices.push_back(animation);
	}

	return animatedMatrices;
}
