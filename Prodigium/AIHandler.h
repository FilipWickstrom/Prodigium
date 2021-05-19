#pragma once
#include "UsefulHeader.h"
#include "Node.h"
class AIHandler
{
private:
	std::vector<Node*> allNodes;
	static AIHandler* instance;
public:
	DELETE_COPY_ASSIGNMENT(AIHandler);
	AIHandler();
	static const bool Initialize();
	virtual ~AIHandler();
	static void CreateNodeGrid();
};

