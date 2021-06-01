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
	std::vector<Node*> path;

	Node* currentEnemyNode;
	EnemyStates states;
	Enemy* monster;
	Player* player;
	int nrOfAstar;
	bool chaseEnabled;
	double stateSwitchTime;
	static AIHandler* instance;
	static std::vector<std::string> OpenFile(std::string filePath);
	AIHandler();
	virtual ~AIHandler();
	Node* GetRandomNode();
	Node* FindClosestNode(const DirectX::SimpleMath::Vector3& position);
	static void AStarSearch();
	static void TracePath(Node* src, Node* dst);
public:
	DELETE_COPY_ASSIGNMENT(AIHandler);
	static const bool Initialize();
	static void CreateNodes();
	static void SetEnemyAndPlayer(Enemy* enemy, Player* player);
	static void ConnectNodes(Node* node1, Node* node2);
	static void MoveEnemy();
	static Node* GetNodeByID(const int& id);
	static const bool& IsChaseEnabled();
	static void ToggleChase();
	static void ToggleEnemySpeed();
	static void Reset();
	static void Destroy();
};

