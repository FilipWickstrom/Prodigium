#pragma once
#include "UsefulStructuresHeader.h"
#include <vector>

class QuadTree
{
private:
	struct QuadNode
	{
		float posX, posY, width;
		int triCount;
		ID3D11Buffer* vBuffer, *iBuffer;
		QuadNode* NW;
		QuadNode* NE;
		QuadNode* SW;
		QuadNode* SE;
	};

	QuadNode* root;
	std::vector<Vertex> vertices;
	int triCount;
	int drawCount;

	bool ContainsTriangle(const float& t1, const float& t2, const float& t3);

public:
	QuadTree() = default;
	~QuadTree() = default;


};