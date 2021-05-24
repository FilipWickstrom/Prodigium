#pragma once
#include "UsefulStructuresHeader.h"
#include <vector>
#include "MeshObject.h"

class QuadTree
{
private:
	static const int CHILD_COUNT = 4;
	const int depth;
	struct QuadNode
	{
		std::vector<MeshObject*> objects;
		Collider collider;
		QuadNode* childs[CHILD_COUNT];

		QuadNode();

		void CalculateChildDimensions(int index, QuadNode* parent);
	};

	// Recursive call to add into Quadtree
	void AddNodes(int level, QuadNode* node);
	void ClearTree(QuadNode* node);
	void AddObject(QuadNode* node);

public:
	QuadTree(int depth = 5);
	~QuadTree();

	int nrOf = 0;

	void BuildQuadTree(const std::vector<MeshObject*>& objects);
	QuadNode* root;
	void DrawableNodes(QuadNode* node, const DirectX::BoundingFrustum& frustum);
};