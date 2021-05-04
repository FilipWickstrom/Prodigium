#include "AnimatedObject.h"
bool AnimatedObject::LoadAnimation(std::string animationFile)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("Models/" + animationFile, 0);

	if (!scene || !scene->mRootNode)
	{
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		return false;
	}

	if (!scene->HasAnimations())
	{
		std::cout << "The file " << animationFile << " does not have any animations..." << std::endl;
		return false;
	}

	//Load in the each "pose"
	
	for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
	{
		std::cout << scene->mRootNode->mChildren[i]->mNumChildren << std::endl;
	}

	/*aiMatrix4x4 test = scene->mRootNode->mChildren[1]->mTransformation;
	std::cout << "Matrix" << std::endl;
	std::cout << test.a1 << " " << test.a2 << " " << test.a3 << " " << test.a4 << std::endl;
	std::cout << test.b1 << " " << test.b2 << " " << test.b3 << " " << test.b4 << std::endl;
	std::cout << test.c1 << " " << test.c2 << " " << test.c3 << " " << test.c4 << std::endl;
	std::cout << test.d1 << " " << test.d2 << " " << test.d3 << " " << test.d4 << std::endl;*/

	/*std::cout << "\nSome animation information" << std::endl;
	std::cout << "Duraction: " << scene->mAnimations[0]->mDuration << std::endl;					//299
	std::cout << "Ticks per second: " << scene->mAnimations[0]->mTicksPerSecond << std::endl;		//30
	std::cout << "Channels: " << scene->mAnimations[0]->mNumChannels << std::endl;*/				//20


	return true;
}

AnimatedObject::AnimatedObject()
{
}

AnimatedObject::~AnimatedObject()
{
}

bool AnimatedObject::Initialize()
{
	//SOME TESTING FOR NOW	
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile("Models/Player/PlayerSkeleton.fbx",
											aiProcess_Triangulate |               //Triangulate every surface
											aiProcess_JoinIdenticalVertices |     //Ignores identical veritices - memory saving
											aiProcess_FlipUVs |                   //Flips the textures to fit directX-style                                              
											aiProcess_FlipWindingOrder |          //Makes it clockwise order
											aiProcess_MakeLeftHanded);			  //Use a lefthanded system for the models 
	
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		return false;
	}

	std::cout << "Got the scene!" << std::endl;
	std::cout << "Number of meshes: " << scene->mNumMeshes << std::endl;
	std::cout << "Number of animations: " << scene->mNumAnimations << std::endl;

	std::cout << "Root information" << std::endl;
	std::cout << "Root number of meshes: " << scene->mRootNode->mNumMeshes << std::endl;
	std::cout << "Root number of childs: " << scene->mRootNode->mNumChildren << std::endl;

	int childs = 0;
	aiNode* currentNode = nullptr;

	if (scene->HasAnimations())
	{
		std::cout << "Has animations" << std::endl;
	}
	else
	{
		std::cout << "Does not have any animations" << std::endl;
	}

	
	//for (int i = 0; i < scene->mRootNode->mNumChildren; i++)
	//{
	//	currentNode = scene->mRootNode->mChildren[i];
	//	std::cout << "Root[" << i << "]" << currentNode->mName.C_Str() << std::endl;



	//	/*currentNode = scene->mRootNode->mChildren[i];
	//	int counter = 0;
	//	while (currentNode != nullptr)
	//	{
	//		std::cout << "Current node has nr of childs: " << currentNode->mNumChildren << std::endl;
	//		if (currentNode->mNumChildren > 0)
	//		{
	//			currentNode = currentNode->mChildren[counter++];
	//		}
	//		else
	//		{
	//			currentNode = currentNode->mParent;
	//		}
	//	}*/

	//}

	if (!LoadAnimation("Player/PlayerIdleWithSkin.fbx"))
	{
		std::cout << "Failed to load animation" << std::endl;
		return false;
	}



	return true;
}
