#include "QuadTree.h"
#include "Engine.h"

void QuadTree::AddNodes(int level, QuadNode* node)
{
	if (level >= this->depth)
	{
		AddObject(node);

		return;
	}

	for (int i = 0; i < CHILD_COUNT; i++)
	{
		node->childs[i] = new QuadNode;
		this->CalculateChildDimensions(i, node, node->childs[i]);
//#ifdef _DEBUG
		this->CreateVertiAndIndices(node->childs[i]);
//#endif
		AddNodes(level + 1, node->childs[i]);
	}
}

void QuadTree::ClearTree(QuadNode* node)
{
	for (int i = 0; i < CHILD_COUNT; i++)
	{
		if (node->childs[i] != nullptr)
		{
			this->ClearTree(node->childs[i]);
		}
	}

//#ifdef _DEBUG
	node->vBuffer->Release();
//#endif

	delete node;
}

void QuadTree::DrawableNodesInternal(QuadNode* node, const DirectX::BoundingFrustum& frustum, std::unordered_map<std::uintptr_t, MeshObject*>& out)
{
	ContainmentType type = node->bounds.Contains(frustum);

	if (type == ContainmentType::DISJOINT)
	{
		return;
	}
//
//#ifdef _DEBUG
	this->nodesToDraw.push_back(node);
//#endif

	for (int i = 0; i < CHILD_COUNT; i++)
	{
		if (node->childs[i] != nullptr)
		{
			DrawableNodesInternal(node->childs[i], frustum, out);
		}
	}

	if (node->childs[0] == nullptr)
	{
		for (int i = 0; i < (int)node->objects.size(); i++)
		{
			std::pair<std::uintptr_t, MeshObject*> toAdd = std::make_pair(reinterpret_cast<std::uintptr_t>(node->objects[i]), node->objects[i]);
			out.emplace(toAdd);
		}
	}
}

void QuadTree::AddObject(QuadNode* node)
{
	using namespace DirectX;
	MeshObject* object;

	for (int i = 0; i < (int)root->objects.size(); i++)
	{
		object = root->objects[i];
		ContainmentType type = node->bounds.Contains(root->objects[i]->modelCollider.boundingBox);
		if (type == ContainmentType::CONTAINS || type == ContainmentType::INTERSECTS)
		{
			node->objects.push_back(object);
		}
	}
}

void QuadTree::BuildQuadTree(const std::vector<MeshObject*>& objects)
{
	if (root == nullptr)
	{
		// Root will get the hardcoded terrains modelCollider which is a collider
		// that covers the entire terrain quad. Next project -> Create terrain class
		// handling terrain will be much easier.
		root = new QuadNode;
		BoundingBox box;
		box.Center = { 0.0f, -5.25f, 0.0f };
		box.Extents = { 750.f, 6.0f, 750.f };
		root->bounds = box;
		root->objects = objects;
//#ifdef _DEBUG
		this->CreateVertiAndIndices(root);
//#endif

		this->AddNodes(0, root);
	}
}

void QuadTree::DrawableNodes(const DirectX::BoundingFrustum& frustum, std::unordered_map<std::uintptr_t, MeshObject*>& out)
{
	if (root == nullptr)
	{
		return;
	}

//#ifdef _DEBUG
	this->nodesToDraw.clear();
//#endif
	this->DrawableNodesInternal(root, frustum, out);
}

//#ifdef _DEBUG
void QuadTree::RenderNodes()
{
	Graphics::GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	for (int i = 0; i < (int)this->nodesToDraw.size(); i++)
	{
		UINT stride = sizeof(Vector3);
		UINT offset = 0;
		Graphics::GetContext()->IASetVertexBuffers(0, 1, &nodesToDraw[i]->vBuffer, &stride, &offset);
		Graphics::GetContext()->Draw(16, 0);
	}
}
//#endif

QuadTree::QuadTree(int depth)
	:depth(depth)
{
	this->root = nullptr;
}

QuadTree::~QuadTree()
{
	if (root != nullptr)
	{
		this->ClearTree(root);
	}
}

QuadTree::QuadNode::QuadNode()
{
//#ifdef _DEBUG
	this->vBuffer = nullptr;
//#endif
	for (int i = 0; i < CHILD_COUNT; i++)
	{
		childs[i] = nullptr;
	}
}

//#ifdef _DEBUG
bool QuadTree::CreateVertiAndIndices(QuadNode* node)
{
	DirectX::XMFLOAT3 corners[8];

	// Returns 8 corners position of bounding box.
	node->bounds.GetCorners(corners);

	std::vector<Vector3> allCorners;

	allCorners.push_back(corners[1]);
	allCorners.push_back(corners[0]);
	allCorners.push_back(corners[3]);
	allCorners.push_back(corners[2]);
	allCorners.push_back(corners[1]);
	allCorners.push_back(corners[5]);
	allCorners.push_back(corners[6]);
	allCorners.push_back(corners[2]);
	allCorners.push_back(corners[3]);
	allCorners.push_back(corners[7]);
	allCorners.push_back(corners[6]);
	allCorners.push_back(corners[7]);
	allCorners.push_back(corners[4]);
	allCorners.push_back(corners[0]);
	allCorners.push_back(corners[4]);
	allCorners.push_back(corners[5]);

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = sizeof(Vector3) * (UINT)allCorners.size();
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA data = {};
	data.pSysMem = &allCorners[0];
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;

	HRESULT hr = Graphics::GetDevice()->CreateBuffer(&desc, &data, &node->vBuffer);
	if (FAILED(hr))
	{
		std::cout << "Failed to create vertex buffer..." << std::endl;
		return false;
	}


	return true;
}
//#endif

void QuadTree::CalculateChildDimensions(int index, QuadNode* parent, QuadNode* child)
{
	using namespace DirectX;

	BoundingBox parentBox = parent->bounds;
	BoundingBox childBox = parentBox;
	float halfLengthX = parentBox.Extents.x * 0.5f;
	float halfLengthZ = parentBox.Extents.z * 0.5f;

	// North West
	if (index == 0)
	{
		childBox.Center.x -= halfLengthX;
		childBox.Center.z -= halfLengthZ;
		childBox.Extents.x = halfLengthX;
		childBox.Extents.z = halfLengthZ;
	}

	// North East
	else if (index == 1)
	{
		childBox.Center.x += halfLengthX;
		childBox.Center.z -= halfLengthZ;
		childBox.Extents.x = halfLengthX;
		childBox.Extents.z = halfLengthZ;
	}

	// South West
	else if (index == 2)
	{
		childBox.Center.x -= halfLengthX;
		childBox.Center.z += halfLengthZ;
		childBox.Extents.x = halfLengthX;
		childBox.Extents.z = halfLengthZ;
	}

	// South East
	else if (index == 3)
	{
		childBox.Center.x += halfLengthX;
		childBox.Center.z += halfLengthZ;
		childBox.Extents.x = halfLengthX;
		childBox.Extents.z = halfLengthZ;
	}

	child->bounds = childBox;
}