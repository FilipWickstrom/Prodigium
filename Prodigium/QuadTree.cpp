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
		this->CalculateChildDimensions(i, node, node->childs[i]);
		AddObject(node->childs[i]);

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
	ContainmentType type = node->bounds.Contains(frustum);

	if (type == ContainmentType::DISJOINT)
	{
		return;
	}

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

	for (int i = 1; i < (int)root->objects.size(); i++)
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
		box.Center = { 0.0f, 0.0f, 0.0f };
		box.Extents = { 750.f, 750.f, 750.f };
		root->bounds = box;
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

	this->DrawableNodesInternal(root, frustum, out);

	std::pair<std::uintptr_t, MeshObject*> toAdd;
	toAdd = std::make_pair(reinterpret_cast<std::uintptr_t>(root->objects[0]), root->objects[0]);
	out.emplace(toAdd);

	toAdd = std::make_pair(reinterpret_cast<std::uintptr_t>(root->objects[1]), root->objects[1]);
	out.emplace(toAdd);
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