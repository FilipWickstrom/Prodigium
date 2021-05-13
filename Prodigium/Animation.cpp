#include "Animation.h"
using namespace DirectX::SimpleMath;

Animation::Animation()
{
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
	importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false); //To remove $AssimpFBX...???***
	const aiScene* scene = importer.ReadFile("Models/" + filename,
											aiProcess_FlipWindingOrder |
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

	//NOT ALL GETS LOADED... ONLY 20/22 "mixamorig:LeftHand" & "mixamorig:RightHand" left...

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
			std::cout << "Added animation matrices for: " << boneName << std::endl;
			
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
					rotation = channel->mRotationKeys[key].mValue;	//Normalize???
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
		}

	}

	importer.FreeScene();
	return true;
}

bool Animation::GetAnimationMatrix(unsigned int keyFrame, std::string boneName, Matrix& animation)
{
	//Returns true if we got an animation matrix on that bone.
	//Returns the matrix in paramater. 
	//All positions, scale and rotation is absolute in that space
	
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
