#include "QuadTree.h"
#include "Engine.h"

void QuadTree::AddNodes(int level, QuadNode* node)
{
	if (level >= this->depth)
	{
		return;
	}

	for (int i = 0; i < CHILD_COUNT; i++)
	{
		node->childs[i] = new QuadNode;
		node->CalculateChildDimensions(i, node);
		if (node != root)
		{
			AddObject(node);
		}

		AddNodes(level + 1, node->childs[i]);
	}
}

void QuadTree::ClearTree(QuadNode* node)
{
	if (node == nullptr)
	{
		return;
	}

	for (int i = 0; i < CHILD_COUNT; i++)
	{
		if (node->childs[i] != nullptr)
		{
			this->ClearTree(node->childs[i]);
		}
	}

	delete node;
}

void QuadTree::DrawableNodesInternal(QuadNode* node, const DirectX::BoundingFrustum& frustum, std::unordered_map<std::uintptr_t, MeshObject*>& out)
{
	if (node == nullptr)
	{
		return;
	}

	ContainmentType type = node->collider.boundingBox.Contains(frustum);

	if (type == ContainmentType::DISJOINT)
	{
		return;
	}

	for (int i = 0; i < CHILD_COUNT; i++)
	{
		DrawableNodesInternal(node->childs[i], frustum, out);
	}

}

void QuadTree::AddObject(QuadNode* node)
{
	using namespace DirectX;
	MeshObject* object;

	for (int i = 1; i < (int)root->objects.size(); i++)
	{
		object = root->objects[i];
		ContainmentType type = node->collider.boundingBox.Contains(root->objects[i]->modelCollider.boundingBox);
		if (type == ContainmentType::CONTAINS)
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
		root->collider = objects[1]->modelCollider;
		root->collider.boundingBox.Extents.y *= 750.f;
		root->objects = objects;

		this->AddNodes(0, root);
	}
}

void QuadTree::DrawableNodes(const DirectX::BoundingFrustum& frustum, std::unordered_map<std::uintptr_t, MeshObject*>& out)
{
	if (root == nullptr)
	{
		return;
	}

	for (int i = 0; i < CHILD_COUNT; i++)
	{
		this->DrawableNodesInternal(root, frustum, out);
	}

	//for (int i = 0; i < (int)node->objects.size(); i++)
	//{
	//	std::pair<std::uintptr_t, MeshObject*> toAdd = std::make_pair(reinterpret_cast<std::uintptr_t>(node->objects[i]), node->objects[i]);
	//	out.emplace(toAdd);
	//}
}

QuadTree::QuadTree(int depth)
	:depth(depth)
{
	this->root = nullptr;
}

QuadTree::~QuadTree()
{
	this->ClearTree(root);
}

QuadTree::QuadNode::QuadNode()
{
	for (int i = 0; i < CHILD_COUNT; i++)
	{
		childs[i] = nullptr;
	}
}

void QuadTree::QuadNode::CalculateChildDimensions(int index, QuadNode* parent)
{
	using namespace DirectX;

	BoundingOrientedBox parentBox = parent->collider.boundingBox;
	BoundingOrientedBox child = parentBox;
	float halfLengthX = parentBox.Extents.x * 0.5f;
	float halfLengthZ = parentBox.Extents.z * 0.5f;

	// North West
	if (index == 0)
	{
		child.Center.x -= halfLengthX;
		child.Center.z -= halfLengthZ;
		child.Extents.x = halfLengthX;
		child.Extents.z = halfLengthZ;
		parent->childs[index]->collider.boundingBox = child;
		return;
	}

	// North East
	if (index == 1)
	{
		child.Center.x += halfLengthX;
		child.Center.z -= halfLengthZ;
		child.Extents.x = halfLengthX;
		child.Extents.z = halfLengthZ;
		parent->childs[index]->collider.boundingBox = child;
		return;
	}

	// South West
	if (index == 2)
	{
		child.Center.x -= halfLengthX;
		child.Center.z += halfLengthZ;
		child.Extents.x = halfLengthX;
		child.Extents.z = halfLengthZ;
		parent->childs[index]->collider.boundingBox = child;
		return;
	}

	// South East
	if (index == 3)
	{
		child.Center.x += halfLengthX;
		child.Center.z += halfLengthZ;
		child.Extents.x = halfLengthX;
		child.Extents.z = halfLengthZ;
		parent->childs[index]->collider.boundingBox = child;
		return;
	}
}