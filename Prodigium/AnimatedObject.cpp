#include "AnimatedObject.h"
using namespace DirectX::SimpleMath;

bool AnimatedObject::LoadVertexShader()
{
	std::string shaderData;
	std::ifstream reader;

	reader.open("AnimationVertexShader.cso", std::ios::binary | std::ios::ate);
	if (!reader.is_open())
	{
		return false;
	}
	reader.seekg(0, std::ios::end);
	shaderData.reserve(static_cast<unsigned int>(reader.tellg()));
	reader.seekg(0, std::ios::beg);

	shaderData.assign((std::istreambuf_iterator<char>(reader)), std::istreambuf_iterator<char>());

	HRESULT hr = Graphics::GetDevice()->CreateVertexShader(shaderData.c_str(), shaderData.length(), nullptr, &this->vertexShader);

	this->vShaderByteCode = shaderData;
	shaderData.clear();
	reader.close();

	return !FAILED(hr);
}

bool AnimatedObject::CreateInputLayout()
{
	D3D11_INPUT_ELEMENT_DESC geometryLayout[5] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEIDS", 0, DXGI_FORMAT_R32G32B32A32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
	};

	HRESULT hr = Graphics::GetDevice()->CreateInputLayout(geometryLayout, 5, this->vShaderByteCode.c_str(), 
														  this->vShaderByteCode.length(), &this->inputlayout);

	return !FAILED(hr);
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

bool AnimatedObject::LoadRiggedMesh(std::string riggedModelFile)
{
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile("Models/" + riggedModelFile,
											aiProcess_Triangulate |               //Triangulate every surface
											aiProcess_JoinIdenticalVertices |     //Ignores identical veritices - memory saving
											aiProcess_FlipUVs |                   //Flips the textures to fit directX-style                                              
											aiProcess_FlipWindingOrder |          //Makes it clockwise order
											aiProcess_MakeLeftHanded |			  //Use a lefthanded system for the models 
											aiProcess_LimitBoneWeights |		  //Limit to 4 weights per vertex
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
					std::cout << "Skeleton: '" << riggedModelFile << "' has " << mesh->mNumBones << " bones" << std::endl;
				#endif

				std::vector<AnimationVertex> vertices;
				vertices.reserve(mesh->mNumVertices);

				/*--------Loading all vertices for the mesh---------*/
				for (UINT i = 0; i < mesh->mNumVertices; i++)
				{
					AnimationVertex temp;
					temp.position = { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z };
					temp.normal = { mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z };
					temp.uv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
					//Bone ID and Weights are set to 0 by default
					vertices.push_back(temp);
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
			std::cout << "The file: " << riggedModelFile << " does not have any bones..." << std::endl;
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

bool AnimatedObject::LoadAnimations(std::string riggedModelFile)
{
	//Search for riggedModel+idle
	//Search for riggedModel+walking
	//Search for riggedModel+running


	//this->allAnimations.push_back();

	//LoadAnimation and add to the array
	Animation animRun;
	if (!animRun.Load("Player/PlayerWalk.fbx", this->boneMap, 150))
	{
		std::cout << "Failed to load running animation..." << std::endl;
		return false;
	}
	this->allAnimations[UINT(AnimationState::RUN)] = animRun;


	return true;
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

bool AnimatedObject::LoadTextures(std::string diffuse, std::string normalMap)
{
	diffuse = "Textures/" + diffuse;

	//Load in the diffuse texture
	ID3D11Texture2D* diffTexture = ResourceManager::GetTexture(diffuse);
	if (diffTexture == nullptr)
	{
		std::cout << "Failed to get diffuse texture from resourceManager..." << std::endl;
		return false;
	}
	HRESULT hr = Graphics::GetDevice()->CreateShaderResourceView(diffTexture, nullptr, &this->textureSRVs[0]);
	if (FAILED(hr))
	{
		std::cout << "Diffuse texture failed to bind to shader resource view..." << std::endl;
		return false;
	}
	
	if (normalMap != "")
	{
		ID3D11Texture2D* normTexture = ResourceManager::GetTexture(normalMap);
		if (normTexture == nullptr)
		{
			std::cout << "Failed to get normal texture from resourceManager..." << std::endl;
			return false;
		}
		HRESULT hr = Graphics::GetDevice()->CreateShaderResourceView(normTexture, nullptr, &this->textureSRVs[1]);
		if (FAILED(hr))
		{
			std::cout << "Normal texture failed to bind to shader resource view..." << std::endl;
			return false;
		}
	}

	return true;
}

void AnimatedObject::CalcFinalMatrix(Bone& currentBone, UINT parentID)
{
	Matrix localMatrix = this->animatedMatrices[this->boneMap[currentBone.name]];
	UINT id = currentBone.id;

	//Rootbone
	if (parentID == -1)
	{
		//Get GameObjects matrix with scale, rotation and translation and multiply with localMatrix
		this->modelMatrices[id] = GetTransposedMatrix() * localMatrix;
	}
	else
	{
		this->modelMatrices[id] = this->modelMatrices[parentID] * localMatrix;
	}

	this->finalMatrices[id] = this->modelMatrices[id] * currentBone.inverseBind;

	for (size_t i = 0; i < currentBone.children.size(); i++)
	{
		CalcFinalMatrix(currentBone.children[i], currentBone.id);
	}
}


AnimatedObject::AnimatedObject()
	:GameObject()
{
	this->vertexShader = nullptr;
	this->inputlayout = nullptr;
	this->vertexBuffer = nullptr;
	this->indexBuffer = nullptr;
	this->indexCount = 0;
	this->boneMatricesBuffer = nullptr;

	for (UINT i = 0; i < MAXTEXTURES; i++)
	{
		this->textureSRVs[i] = nullptr;
	}

	this->modelMatrices.resize(MAXBONES, Matrix::Identity);
	this->animatedMatrices.resize(MAXBONES, Matrix::Identity);
	this->finalMatrices.resize(MAXBONES, Matrix::Identity);

	this->currentState = AnimationState::RUN;
	this->allAnimations.resize(size_t(AnimationState::NROFSTATE));
}

AnimatedObject::~AnimatedObject()
{
	if (this->vertexShader)
		this->vertexShader->Release();
	if (this->inputlayout)
		this->inputlayout->Release();
	if (this->vertexBuffer)
		this->vertexBuffer->Release();
	if (this->indexBuffer)
		this->indexBuffer->Release();
	if (this->boneMatricesBuffer)
		this->boneMatricesBuffer->Release();

	for (UINT i = 0; i < MAXTEXTURES; i++)
	{
		if (this->textureSRVs[i])
			this->textureSRVs[i]->Release();
	}
}

bool AnimatedObject::Initialize(std::string tposeFile, std::string diffuse, std::string normalMap, 
								const DirectX::SimpleMath::Vector3& pos,
								const DirectX::SimpleMath::Vector3& rot,
								const DirectX::SimpleMath::Vector3& scl)
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

	BuildMatrix(pos, scl, rot);

	if (!LoadRiggedMesh(tposeFile))
	{
		std::cout << "Failed to load in skeleton" << std::endl;
		return false;
	}

	if (!LoadTextures(diffuse, normalMap))
	{
		std::cout << "Failed to load in textures..." << std::endl;
		return false;
	}

	if (!CreateBonesCBuffer())
	{
		std::cout << "Failed to create cbuffer for bones matrices" << std::endl;
		return false;
	}
	
	if (!LoadAnimations(tposeFile))
	{
		std::cout << "Failed to load one of the animations..." << std::endl;
		return false;
	}

	return true;
}

void AnimatedObject::ChangeAnimState(AnimationState state)
{
	this->currentState = state;
}

void AnimatedObject::Render()
{
	//if (playAnimation)
	//{
	
	//Get all animated matrices at this time for every bone
	this->allAnimations[(UINT)this->currentState].GetAnimationMatrices(this->boneNames, this->animatedMatrices);
	
	//Calculate all matrices that will later be sent to the GPU
	CalcFinalMatrix(this->rootBone, -1);
	
	//Update the array of matrices to the GPU
	UpdateBonesCBuffer();

	//}

	Graphics::GetContext()->VSSetConstantBuffers(2, 1, &this->boneMatricesBuffer);
	Graphics::GetContext()->VSSetShader(this->vertexShader, nullptr, 0);
	Graphics::GetContext()->IASetInputLayout(this->inputlayout);

	UINT stride = sizeof(AnimationVertex);
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &this->vertexBuffer, &stride, &offset);
	Graphics::GetContext()->IASetIndexBuffer(this->indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//Set textures to pixelshader
	for (unsigned int i = 0; i < MAXTEXTURES; i++)
	{
		Graphics::GetContext()->PSSetShaderResources(i, 1, &this->textureSRVs[i]);
	}

	//Finally draw the mesh
	Graphics::GetContext()->DrawIndexed(this->indexCount, 0, 0);
}
