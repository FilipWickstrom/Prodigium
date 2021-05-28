#include "AnimatedObject.h"
using namespace DirectX::SimpleMath;

bool AnimatedObject::LoadVertexShader()
{
	std::string shaderData;
	std::ifstream reader;

	reader.open("AnimationVS.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	HRESULT hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &this->vertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	this->vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();

	reader.open("AnimationShadowVS.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &this->shadowVertexShader);
	if (FAILED(hr))
	{
		return false;
	}
	
	this->shadowVShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();
	return true;
}

bool AnimatedObject::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC geometryLayout[7] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = Graphics::GetDevice()->CreateInputLayout(geometryLayout, 7, this->vShaderByteCode.c_str(), 
														  this->vShaderByteCode.length(), &this->inputlayout);

	if (FAILED(hr))
	{
		return false;
	}

	D3D11_INPUT_ELEMENT_DESC shadowLayout[3] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	hr = Graphics::GetDevice()->CreateInputLayout(shadowLayout, 3, this->shadowVShaderByteCode.c_str(),
														this->shadowVShaderByteCode.length(), &this->shadowInputlayout);

	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

bool AnimatedObject::CreateVertIndBuffers(const std::vector<AnimationVertex>& vertices, const std::vector<UINT>& indices, UINT nrOfIndices)
{
	/*-----Vertexbuffer-----*/
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(AnimationVertex) * (UINT)vertices.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &vertices[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->vertexBuffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create vertex buffer..." << std::endl;
		return false;
	}

	/*-----Indexbuffer-----*/
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(UINT) * nrOfIndices;
	data.pSysMem = &indices[0];

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->indexBuffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create index buffer..." << std::endl;
		return false;
	}

	return true;
}

bool AnimatedObject::LoadBoneTree(Bone& currentBone, aiNode* node, std::unordered_map<std::string, std::pair<UINT, Matrix>>&tempMap)
{
	//Search in the temp bone map after the name from the node
	if (tempMap.find(node->mName.C_Str()) != tempMap.end())
	{
		//Take the node first, then later children
		currentBone.name = node->mName.C_Str();
		currentBone.id = tempMap[currentBone.name].first;
		currentBone.inverseBind = tempMap[currentBone.name].second;
	
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			Bone child;
			//Only adds child to the current bone if it successfully was added
			if (LoadBoneTree(child, node->mChildren[i], tempMap))
			{
				currentBone.children.push_back(child);
			}
		}
		return true;
	}
	//Did not find the node in the temp bone map. Check the children
	else
	{
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			if (LoadBoneTree(currentBone, node->mChildren[i], tempMap))
				return true;
		}
	}
	return false;
}

bool AnimatedObject::LoadRiggedMesh(std::string animFolder)
{
	Assimp::Importer importer;

	std::string skeletonFile = animFolder + "/" + animFolder + "_Skeleton.fbx";

	const aiScene* scene = importer.ReadFile("Models/" + skeletonFile,
											aiProcess_Triangulate |               //Triangulate every surface
											aiProcess_JoinIdenticalVertices |     //Ignores identical veritices - memory saving
											aiProcess_FlipUVs |                   //Flips the textures to fit directX-style                                              
											aiProcess_FlipWindingOrder |          //Makes it clockwise order
											aiProcess_MakeLeftHanded |			  //Use a lefthanded system for the models 
											aiProcess_LimitBoneWeights |		  //Limit to 4 weights per vertex
											aiProcess_CalcTangentSpace |          //Fix tangents automatic for us
											0);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ASSIMP ERROR: " << importer.GetErrorString() << std::endl;
		importer.FreeScene();
		return false;
	}

	//Only works with one mesh at this time
	if (scene->mNumMeshes == UINT(1))
	{
		const aiMesh* mesh = scene->mMeshes[0];
		
		//Only accepts files that has bones
		if (mesh->HasBones())
		{
			//Has to have under the max bones
			if (mesh->mNumBones <= MAXBONES)
			{
				#ifdef _DEBUG
					std::cout << "Skeleton: '" << skeletonFile << "' has " << mesh->mNumBones << " bones" << std::endl;
				#endif

				std::vector<AnimationVertex> vertices;
				vertices.reserve(mesh->mNumVertices);

				//Load in material
				const aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
				float shiniess = 0.0f;
				aiColor4D specular = { 0.0f,0.0f,0.0f,0.0f };

				if (material != nullptr)
				{
					aiGetMaterialFloat(material, AI_MATKEY_SHININESS, &shiniess);
					aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
				}

				/*--------Loading all vertices for the mesh---------*/
				for (UINT i = 0; i < mesh->mNumVertices; i++)
				{
					AnimationVertex temp;
					temp.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					temp.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
					temp.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
					temp.tangent = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
					//Add material
					temp.specular = { specular.r, specular.g, specular.b, shiniess };
					//Bone ID and Weights are set to 0 by default
					vertices.push_back(temp);

					//Will be used to define the collisionbox
					this->meshPositions.push_back(temp.position);
				}

				/*--------Loading all bones for the mesh---------*/
				std::vector<UINT>boneCounts;
				boneCounts.resize(vertices.size(), 0);

				//Save down the bones temporarly
				std::unordered_map < std::string, std::pair<UINT, Matrix>> tempBoneMap;

				//Goes through all bones
				for (UINT i = 0; i < mesh->mNumBones; i++)
				{
					aiBone* bone = mesh->mBones[i];

					//Load the temp bonemap with index (same as ID) and matrices 
					tempBoneMap[bone->mName.C_Str()] = { i, Matrix(&bone->mOffsetMatrix.a1) };
					this->boneMap[bone->mName.C_Str()] = i;
					this->boneNames.push_back(bone->mName.C_Str());

					//Goes through all the vertices that the bone affect
					for (UINT v = 0; v < bone->mNumWeights; v++)
					{
						UINT id = bone->mWeights[v].mVertexId;
						float weight = bone->mWeights[v].mWeight;

						//Increase number of bones for this vertex
						boneCounts[id]++;
						switch (boneCounts[id])
						{
						case 1:
							vertices[id].boneIDs.x = i;
							vertices[id].boneWeights.x = weight;
							break;
						case 2:
							vertices[id].boneIDs.y = i;
							vertices[id].boneWeights.y = weight;
							break;
						case 3:
							vertices[id].boneIDs.z = i;
							vertices[id].boneWeights.z = weight;
							break;
						case 4:
							vertices[id].boneIDs.w = i;
							vertices[id].boneWeights.w = weight;
							break;
						default:
							//Ignore adding id or weight as it
							//already has 4 bones connected
							break;
						}
					}
				}

				//Normalize the weights
				for (size_t i = 0; i < vertices.size(); i++)
				{
					Vector4 weights = vertices[i].boneWeights;
					//Total weight
					float total = weights.x + weights.y + weights.z + weights.w;
					//Avoid devide by 0
					if (total > 0.0f)
					{
						vertices[i].boneWeights = { weights.x / total, weights.y / total, weights.z / total, weights.w / total };
					}
				}

				/*--------Loading all faces for the mesh---------*/
				std::vector<UINT> indices;
				indices.reserve(size_t(mesh->mNumFaces) * 3);
				this->indexCount = (UINT)(mesh->mNumFaces * 3);

				//Loading in the indices
				for (UINT i = 0; i < mesh->mNumFaces; i++)
				{
					const aiFace face = mesh->mFaces[i];
					//Only accepts 3 vertices per face
					if (face.mNumIndices == 3)
					{
						indices.push_back(face.mIndices[0]);
						indices.push_back(face.mIndices[1]);
						indices.push_back(face.mIndices[2]);
					}
				}

				//Create buffers for vertices and indices!
				if (!CreateVertIndBuffers(vertices, indices, this->indexCount))
				{
					std::cout << "Failed to create vertex or indexbuffer..." << std::endl;
					importer.FreeScene();
					return false;
				}

				//Load our tree of bones from the scenes rootnode
				LoadBoneTree(this->rootBone, scene->mRootNode, tempBoneMap);
			}
			else
			{
				std::cout << "This object has: " << mesh->mNumBones << " and this engine only supports: " << MAXBONES << std::endl;
				importer.FreeScene();
				return false;
			}
		}
		else
		{
			std::cout << "The file: " << skeletonFile << " does not have any bones..." << std::endl;
			importer.FreeScene();
			return false;
		}
	}
	else
	{
		std::cout << "Only supports one mesh at this time..." << std::endl;
		importer.FreeScene();
		return false;
	}

	importer.FreeScene();
	return true;
}

void AnimatedObject::LoadAnimations(std::string animFolder)
{
	std::string forwardFile = animFolder + "/" + animFolder + "_Forward.fbx";
	std::string backwardFile = animFolder + "/" + animFolder + "_Backward.fbx";
	std::string idleFile = animFolder + "/" + animFolder + "_Idle.fbx";
	std::string idle2File = animFolder + "/" + animFolder + "_Idle2.fbx";
	std::string strafeFile = animFolder + "/" + animFolder + "_Strafe.fbx";
	std::string deathFile = animFolder + "/" + animFolder + "_Dead.fbx";
	std::string pickupFile = animFolder + "/" + animFolder + "_Pickup.fbx";

	UINT counter = 0;

	//Load in forward movement animation
	Animation* forwardAnim = new Animation();
	if (forwardAnim->Load(forwardFile, this->boneMap, true, 120))
	{
		this->animationList.push_back(forwardAnim);
		this->animStates[AnimationState::WALKFORWARD] = counter;
		this->animStates[AnimationState::RUNFORWARD] = counter;
		counter++;
	}
	else
	{
		delete forwardAnim;
	}

	//Load in backward movement animation
	Animation* backwardAnim = new Animation();
	if (backwardAnim->Load(backwardFile, this->boneMap, true, 120))
	{
		this->animationList.push_back(backwardAnim);
		this->animStates[AnimationState::WALKBACKWARD] = counter;
		this->animStates[AnimationState::RUNBACKWARD] = counter;
		counter++;
	}
	else
	{
		delete backwardAnim;
	}

	//Load in idle 1 animation
	Animation* idleAnim = new Animation();
	if (idleAnim->Load(idleFile, this->boneMap, true, 30))
	{
		this->animationList.push_back(idleAnim);
		this->animStates[AnimationState::IDLE] = counter;
		this->currentState = AnimationState::IDLE;
		counter++;
	}
	else
	{
		delete idleAnim;
	}
	
	//Load in idle 2 animation
	Animation* idle2Anim = new Animation();
	if (idle2Anim->Load(idle2File, this->boneMap, true, 45))
	{
		this->animationList.push_back(idle2Anim);
		this->animStates[AnimationState::IDLE2] = counter;
		counter++;
	}
	else
	{
		delete idle2Anim;
	}

	//Load in strafe animation
	Animation* strafeAnim = new Animation();
	if (strafeAnim->Load(strafeFile, this->boneMap, true, 50))
	{
		this->animationList.push_back(strafeAnim);
		this->animStates[AnimationState::WALKLEFT] = counter;
		this->animStates[AnimationState::RUNLEFT] = counter;
		this->animStates[AnimationState::WALKRIGHT] = counter;
		this->animStates[AnimationState::RUNRIGHT] = counter;
		counter++;
	}
	else
	{
		delete strafeAnim;
	}

	//Load in death animation
	Animation* deathAnim = new Animation();
	if (deathAnim->Load(deathFile, this->boneMap, false, 30))
	{
		this->animationList.push_back(deathAnim);
		this->animStates[AnimationState::DEAD] = counter;
		counter++;
	}
	else
	{
		delete deathAnim;
	}

	//Load in pickup animation
	Animation* pickupAnim = new Animation();
	if (pickupAnim->Load(pickupFile, this->boneMap, false, 250))
	{
		this->animationList.push_back(pickupAnim);
		this->animStates[AnimationState::PICKUP] = counter;
		counter++;
	}
	else
	{
		delete pickupAnim;
	}
}

bool AnimatedObject::CreateBonesCBuffer()
{
	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.ByteWidth = sizeof(Matrix) * MAXBONES;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &this->finalMatrices[0];

	return !FAILED(Graphics::GetDevice()->CreateBuffer(&desc, &data, &this->boneMatricesBuffer));
}

void AnimatedObject::UpdateBonesCBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Graphics::GetContext()->Map(this->boneMatricesBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &this->finalMatrices[0], MAXBONES * sizeof(Matrix));
	Graphics::GetContext()->Unmap(this->boneMatricesBuffer, 0);
}

void AnimatedObject::CalcFinalMatrix(Bone& currentBone, UINT parentID, const DirectX::SimpleMath::Matrix& worldMatrix)
{
	Matrix localMatrix = this->animatedMatrices[this->boneMap[currentBone.name]];
	UINT id = currentBone.id;

	//Rootbone
	if (parentID == -1)
	{
		//Get GameObjects matrix with scale, rotation and translation and multiply with localMatrix
		this->modelMatrices[id] = worldMatrix * localMatrix;
	}
	else
	{
		this->modelMatrices[id] = this->modelMatrices[parentID] * localMatrix;
	}

	this->finalMatrices[id] = this->modelMatrices[id] * currentBone.inverseBind;

	for (size_t i = 0; i < currentBone.children.size(); i++)
	{
		CalcFinalMatrix(currentBone.children[i], currentBone.id, worldMatrix);
	}
}

void AnimatedObject::ClearTree(Bone& currentBone)
{
	for (int i = 0; i < currentBone.children.size(); i++)
	{
		if (!currentBone.children.empty())
		{
			ClearTree(currentBone.children[i]);
			currentBone.children[i].children.clear();
		}
	}
	currentBone.children.clear();
}

AnimatedObject::AnimatedObject()
	:Resource(ResourceType::ANIMATEDOBJ)
{
	this->vertexShader = nullptr;
	this->inputlayout = nullptr;
	this->shadowVertexShader = nullptr;
	this->shadowInputlayout = nullptr;
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->indexCount = 0;
	this->boneMatricesBuffer = nullptr;

	this->modelMatrices.resize(MAXBONES, Matrix::Identity);
	this->animatedMatrices.resize(MAXBONES, Matrix::Identity);
	this->finalMatrices.resize(MAXBONES, Matrix::Identity);
	this->currentState = AnimationState::NONE;
	this->useInterpolation = true;
}

AnimatedObject::~AnimatedObject()
{
	if (this->vertexShader)
		this->vertexShader->Release();
	if (this->inputlayout)
		this->inputlayout->Release();
	if (this->shadowVertexShader)
		this->shadowVertexShader->Release();
	if (this->shadowInputlayout)
		this->shadowInputlayout->Release();
	if (this->vertexBuffer)
		this->vertexBuffer->Release();
	if (this->indexBuffer)
		this->indexBuffer->Release();
	if (this->boneMatricesBuffer)
		this->boneMatricesBuffer->Release();

	this->animatedMatrices.clear();
	this->modelMatrices.clear();
	this->finalMatrices.clear();

	for (int i = 0; i < this->animationList.size(); i++)
	{
		if (this->animationList[i] != nullptr)
		{
			delete this->animationList[i];
		}
	}

	this->vShaderByteCode.clear();
	this->shadowVShaderByteCode.clear();
	this->animationList.clear();	//Need to delete inside of it?
	this->animStates.clear();
	this->boneMap.clear();
	this->meshPositions.clear();
	this->boneNames.clear();

	//Release everything in bone tree
	ClearTree(this->rootBone);
}

bool AnimatedObject::Initialize(std::string animFolder)
{
	if (!LoadVertexShader())
	{
		std::cout << "Failed to load vertexshader in animated object..." << std::endl;
		return false;
	}

	if (!CreateInputLayout())
	{
		std::cout << "Failed to create inputlayout for the animated object..." << std::endl;
		return false;
	}

	if (!LoadRiggedMesh(animFolder))
	{
		std::cout << "Failed to load in skeleton" << std::endl;
		return false;
	}

	if (!CreateBonesCBuffer())
	{
		std::cout << "Failed to create cbuffer for bones matrices" << std::endl;
		return false;
	}
	
	LoadAnimations(animFolder);

	return true;
}

void AnimatedObject::ChangeAnimState(AnimationState state)
{
	//Change animation if we are not already on it
	if (this->currentState != state)
	{
		//Check if the animation even exists
		if (this->animStates.find(state) != this->animStates.end())
		{
			AnimationState previousState = this->currentState;
			
			//Reset values from previous states
			switch (previousState)
			{
			case AnimationState::IDLE:
			case AnimationState::IDLE2:
				this->animationList[this->animStates[previousState]]->ResetCurrentTime();
				break;
			case AnimationState::DEAD:
			case AnimationState::PICKUP:
				this->animationList[this->animStates[previousState]]->ResetCurrentTime();
				this->animationList[this->animStates[previousState]]->ResetCurrentTime();
				break;
			default:
				break;
			};

			this->currentState = state;

			//Change speed on some animations that share the same file
			switch (this->currentState)
			{
			case AnimationState::WALKFORWARD:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(120);
				break;
			case AnimationState::RUNFORWARD:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(200);
				break;
			case AnimationState::WALKBACKWARD:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(120);
				break;
			case AnimationState::RUNBACKWARD:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(200);
				break;
			case AnimationState::WALKLEFT:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(-75);
				break;
			case AnimationState::RUNLEFT:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(-110);
				break;
			case AnimationState::WALKRIGHT:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(75);
				break;
			case AnimationState::RUNRIGHT:
				this->animationList[this->animStates[this->currentState]]->SetAnimationSpeed(110);
				break;
			default:
				break;
			};
		}
	}
	
}

void AnimatedObject::UseInterpolation(bool toggle)
{
	this->useInterpolation = toggle;
}

bool AnimatedObject::AnimationReachedEnd()
{
	return this->animationList[this->animStates[this->currentState]]->HasReachedEnd();
}

const AnimationState& AnimatedObject::GetAnimationState()
{
	return this->currentState;
}

void AnimatedObject::Render(const DirectX::SimpleMath::Matrix& worldMatrix, bool animate)
{	
	if (!GUIHandler::IsPaused())
	{
		//Animate the object and get the new matrices
		if (animate && this->currentState != AnimationState::NONE)
		{
			//Get all animated matrices at this time for every bone
			this->animationList[this->animStates[this->currentState]]->GetAnimationMatrices(this->boneNames, this->animatedMatrices, this->useInterpolation);

			//Calculate all matrices that will later be sent to the GPU
			CalcFinalMatrix(this->rootBone, -1, worldMatrix);

			//Update the array of matrices to the GPU
			UpdateBonesCBuffer();
		}
	}

	Graphics::GetContext()->VSSetConstantBuffers(6, 1, &this->boneMatricesBuffer);
	Graphics::GetContext()->VSSetShader(this->vertexShader, nullptr, 0);
	Graphics::GetContext()->IASetInputLayout(this->inputlayout);

	UINT stride = sizeof(AnimationVertex);
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	Graphics::GetContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Finally draw the mesh
	Graphics::GetContext()->DrawIndexed(this->indexCount, 0, 0);

	ID3D11Buffer* nullBoneBuffer = nullptr;
	Graphics::GetContext()->VSSetConstantBuffers(6, 1, &nullBoneBuffer);
}

void AnimatedObject::RenderShadows(const DirectX::SimpleMath::Matrix& worldMatrix)
{
	Graphics::GetContext()->VSSetConstantBuffers(6, 1, &this->boneMatricesBuffer);
	Graphics::GetContext()->VSSetShader(this->shadowVertexShader, nullptr, 0);
	Graphics::GetContext()->IASetInputLayout(this->shadowInputlayout);

	UINT stride = sizeof(AnimationVertex);
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	Graphics::GetContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Finally draw the mesh
	Graphics::GetContext()->DrawIndexed(this->indexCount, 0, 0);

	ID3D11Buffer* nullBoneBuffer = nullptr;
	Graphics::GetContext()->VSSetConstantBuffers(6, 1, &nullBoneBuffer);
}
