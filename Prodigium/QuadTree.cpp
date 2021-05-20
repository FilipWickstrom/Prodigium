#include "QuadTree.h"

void QuadTree::AddObject(MeshObject* object)
{

}

void QuadTree::BuildQuadTree()
{
	if (root == nullptr)
	{

	}
}

QuadTree::QuadTree(int depth)
	:depth(depth)
{
	this->root = nullptr;
}

QuadTree::QuadNode::QuadNode(int level, const int depth)
{
	if (level >= depth)
	{
		return;
	}

	for (int i = 0; i < 4; i++)
	{
		childs[i] = new QuadNode(level + 1, depth);
	}
}