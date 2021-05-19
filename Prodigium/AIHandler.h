#pragma once
#include "UsefulHeader.h"
#include "Node.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
class AIHandler
{
private:
	std::vector<Node*> allNodes;
	static AIHandler* instance;
	static std::vector<std::string> openFile(std::string filePath);
public:
	DELETE_COPY_ASSIGNMENT(AIHandler);
	AIHandler();
	static const bool Initialize();
	virtual ~AIHandler();
	static void CreateNodes();
	static void ConnectNodes(Node* node1, Node* node2);

	static Node* GetNodeByID(const int& id);
};

