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
		DirectX::BoundingBox bounds;
		QuadNode* childs[CHILD_COUNT];

#ifdef _DEBUG
		ID3D11Buffer* vBuffer;
#endif

		QuadNode();
	};

	bool CreateVertiAndIndices(QuadNode* node);

	// Recursive call to add into Quadtree
	void CalculateChildDimensions(int index, QuadNode* parent, QuadNode* child);
	void AddNodes(int level, QuadNode* node);
	void ClearTree(QuadNode* node);
	void AddObject(QuadNode* node);
	void DrawableNodesInternal(QuadNode* node, const DirectX::BoundingFrustum& frustum, std::unordered_map<std::uintptr_t, MeshObject*>& out);
	QuadNode* root;
	
#ifdef _DEBUG
	std::vector<QuadNode*> nodesToDraw;
#endif

public:
	QuadTree(int depth = 5);
	~QuadTree();

	void BuildQuadTree(const std::vector<MeshObject*>& objects);
	void DrawableNodes(const DirectX::BoundingFrustum& frustum, std::unordered_map<std::uintptr_t, MeshObject*>& out);
	
#ifdef _DEBUG
	void RenderNodes();
#endif
};