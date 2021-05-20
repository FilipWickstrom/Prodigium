#pragma once
#include "UsefulStructuresHeader.h"
#include <vector>
#include "MeshObject.h"

class QuadTree
{
private:
	const int depth;
	struct QuadNode
	{
		std::vector<MeshObject*> objects;
		Collider collider;
		QuadNode* childs[4] = { nullptr };

		QuadNode(int level, const int depth);
	};

	QuadNode* root;

	// Recursive call to add into Quadtree
	void AddObject(MeshObject* object);

	void BuildQuadTree();

public:
	QuadTree(int depth = 5);
	~QuadTree() = default;
};