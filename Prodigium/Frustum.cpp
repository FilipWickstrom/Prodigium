#include "Frustum.h"
#include "ResourceManager.h"
#include "Graphics.h"

#ifdef _DEBUG
bool Frustum::CreateVertIndiBuffers()
{
	DirectX::XMFLOAT3 corners[8];

	// Returns 8 corners position of bounding frustum.

	this->transformed.GetCorners(corners);

	//     Near    Far
	//    0----1  4----5
	//    |    |  |    |
	//    |    |  |    |
	//    3----2  7----6

	std::vector<unsigned int> indices;

	// NEAR PLANE
	indices.push_back(0);
	indices.push_back(1);

	indices.push_back(1);
	indices.push_back(2);

	indices.push_back(2);
	indices.push_back(3);

	indices.push_back(3);
	indices.push_back(0);

	// FAR PLANE
	indices.push_back(4);
	indices.push_back(5);

	indices.push_back(5);
	indices.push_back(6);

	indices.push_back(6);
	indices.push_back(7);

	indices.push_back(7);
	indices.push_back(4);

	// BRIDGE
	indices.push_back(0);
	indices.push_back(4);

	indices.push_back(1);
	indices.push_back(5);

	indices.push_back(3);
	indices.push_back(7);

	indices.push_back(2);
	indices.push_back(6);

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(corners);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &corners[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &vBuffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create vertex buffer..." << std::endl;
		return false;
	}

	/*-----Indexbuffer-----*/
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = sizeof(unsigned int) * (int)indices.size();
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;

	data.pSysMem = indices.data();

	hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &iBuffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create index buffer..." << std::endl;
		return false;
	}

	return true;
}
#endif

Frustum::Frustum()
{
#ifdef _DEBUG
	this->vBuffer = nullptr;
	this->iBuffer = nullptr;
#endif
}

Frustum::Frustum(const Frustum& other)
{
#ifdef _DEBUG
	this->vBuffer = nullptr;
	this->iBuffer = nullptr;
	this->transformed = other.transformed;
#endif
}

Frustum::~Frustum()
{
#ifdef _DEBUG
	if (this->vBuffer)
		this->vBuffer->Release();
	if (this->iBuffer)
		this->iBuffer->Release();
#endif
}

void Frustum::Update()
{
	using namespace DirectX;
	CameraObject* playerCam = ResourceManager::GetCamera("PlayerCam");
	Vector3 cameraPos = playerCam->position;
	Vector3 rot = playerCam->rotation;
	Quaternion q = Quaternion::CreateFromYawPitchRoll(rot.y, rot.x, rot.z);

	Matrix transform = Matrix::CreateFromQuaternion(q) * Matrix::CreateTranslation(cameraPos);

	this->frustumColliderOriginal.Transform(this->transformed, transform);
}

void Frustum::Drawable(QuadTree*& quadTree, std::unordered_map<std::uintptr_t, MeshObject*>& out)
{
	quadTree->DrawableNodes(this->transformed, out);
}

bool Frustum::Initialize()
{
	CameraObject* playerCam = ResourceManager::GetCamera("PlayerCam");

	DirectX::BoundingFrustum::CreateFromMatrix(this->frustumColliderOriginal, playerCam->GetProjMatrixCPU());

	//SHOWCASE PURPOSE
	//Matrix transform = Matrix::CreateTranslation({ 0.0f, 0.0f, 25.f });
	Matrix transform = Matrix::CreateTranslation(playerCam->position);

	this->frustumColliderOriginal.Transform(this->frustumColliderOriginal, transform);
	this->frustumColliderOriginal.Far = 250.f;

	this->transformed = frustumColliderOriginal;

#ifdef _DEBUG
	if (!CreateVertIndiBuffers())
	{
		return false;
	}
#endif

	return true;
}

#ifdef _DEBUG
void Frustum::Render()
{
	D3D11_MAPPED_SUBRESOURCE mappedData = {};

	DirectX::XMFLOAT3 corners[8];

	// Returns 8 corner position of bounding frustum.
	this->transformed.GetCorners(corners);

	Graphics::GetContext()->Map(vBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	memcpy(mappedData.pData, corners, sizeof(corners));
	Graphics::GetContext()->Unmap(vBuffer, 0);
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	Graphics::GetContext()->IASetIndexBuffer(iBuffer, DXGI_FORMAT_R32_UINT, 0);
	UINT stride = sizeof(corners[0]);
	UINT offset = 0;
	Graphics::GetContext()->IASetVertexBuffers(0, 1, &vBuffer, &stride, &offset);
	Graphics::GetContext()->DrawIndexed(24, 0, 0);
}
#endif