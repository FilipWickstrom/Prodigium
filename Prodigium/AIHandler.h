#pragma once
#include "UsefulHeader.h"
#include "UsefulStructuresHeader.h"
#include "Node.h"
#include "Enemy.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
class AIHandler
{
private:
	std::vector<Node*> allNodes;
	Node* currentEnemyNode;
	EnemyStates states;
	Enemy* monster;
	static AIHandler* instance;
	static std::vector<std::string> openFile(std::string filePath);
public:
	DELETE_COPY_ASSIGNMENT(AIHandler);
	AIHandler();
	static const bool Initialize(Enemy* monster);
	virtual ~AIHandler();
	static void CreateNodes();
	static void ConnectNodes(Node* node1, Node* node2);
	static void moveEnemy(const float& deltaTime);
	static Node* GetNodeByID(const int& id);
};

