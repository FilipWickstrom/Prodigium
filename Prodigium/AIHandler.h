#pragma once
#include "UsefulHeader.h"
#include "UsefulStructuresHeader.h"
#include "Node.h"
#include "Player.h"
#include "Enemy.h"
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <omp.h>
#define AIHANDLER AIHandler::instance
class AIHandler
{
private:
	std::vector<Node*> allNodes;
	Node* currentEnemyNode;
	EnemyStates states;
	Enemy* monster;
	Player* player;
	double stateSwitchTime;
	static AIHandler* instance;
	static std::vector<std::string> openFile(const std::string& filePath);
	Node* FindClosestNode(const DirectX::SimpleMath::Vector3& position);
public:
	DELETE_COPY_ASSIGNMENT(AIHandler);
	AIHandler();
	static const bool Initialize();
	virtual ~AIHandler();
	static void CreateNodes();
	static void SetEnemyAndPlayer(Enemy* enemy, Player* player);
	static void ConnectNodes(Node* node1, Node* node2);
	static void MoveEnemy(const float& deltaTime);
	static Node* GetNodeByID(const int& id);
};

