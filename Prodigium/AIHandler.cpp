#include "AIHandler.h"
AIHandler* AIHANDLER = nullptr;
std::vector<std::string> AIHandler::openFile(const std::string& filePath)
{
	std::ifstream nodeFile;
	nodeFile.open(filePath, std::ios::app);
	std::string currentLine;
	std::vector<std::string> allLines;
	if (nodeFile.is_open())
	{
		while (!nodeFile.eof())
		{
			std::getline(nodeFile, currentLine);
			allLines.push_back(currentLine);
		}
	}
	return allLines;

}

Node* AIHandler::FindClosestNode(const Vector3& position)
{
	Node* currentClosest = AIHANDLER->currentEnemyNode;
	for (Node* currentNode : AIHANDLER->allNodes)
	{
		if ((currentNode->GetPos() - position).Length() < (currentClosest->GetPos() - position).Length())
		{
			currentClosest = currentNode;
		}
	}
	return currentClosest;
}

AIHandler::AIHandler()
{
	currentEnemyNode = nullptr;
	states = EnemyStates::PATROL;
	this->monster = nullptr;
	this->player = nullptr;
	this->stateSwitchTime = 0.f;
}

const bool AIHandler::Initialize()
{
	if (!AIHANDLER)
	{
		AIHANDLER = new AIHandler();

		return true;
	}
	return false;
}

AIHandler::~AIHandler()
{
	if (AIHANDLER)
	{
		delete AIHANDLER;
	}
}

void AIHandler::CreateNodes()
{
	//Todo: Add correct nodes and add their connections
	std::vector<std::string> file = openFile("Resources/Nodes/NodeInfo.txt");//replace with actual name
	int currentIndex = 0;
	if (file.size() == 0)
	{
		std::cout << "Error Opening NodeFile!\n";
	}
	//Creating nodes
	std::cout << "Creating nodes\n";
	while (file[currentIndex] != "")
	{
		std::stringstream ss(file.at(currentIndex));
		int ID = 0;
		float posX = 0, posZ = 0;
		ss >> ID >> posX >> posZ;
		Node* currentNode = new Node();
		currentNode->Initialize({ posX, 0.f, posZ }, ID);
		AIHANDLER->allNodes.push_back(currentNode);
		currentIndex++;
	}
	std::cout << "Connecting Nodes\n";
	for (int i = currentIndex + 1; i < file.size(); i++)
	{
		std::stringstream ss(file.at(i));
		int ID1 = 0, ID2 = 0;
		ss >> ID1 >> ID2;
		AIHANDLER->ConnectNodes(AIHANDLER->GetNodeByID(ID1), AIHANDLER->GetNodeByID(ID2));
	}
	AIHANDLER->currentEnemyNode = AIHANDLER->allNodes.at(0);
}


void AIHandler::SetEnemyAndPlayer(Enemy* enemy, Player* player)
{
	AIHANDLER->monster = enemy;
	AIHANDLER->player = player;
}

void AIHandler::ConnectNodes(Node* node1, Node* node2)
{
	node1->AddConnectedNode(node2);
	node2->AddConnectedNode(node1);
}

void AIHandler::MoveEnemy(const float& deltaTime)
{
	if (AIHANDLER->monster)
	{
		Vector3 DirectionVec;
		switch (AIHANDLER->states)
		{
		case EnemyStates::PATROL:
			if (AIHANDLER->monster->HasReachedTarget())
			{
				AIHANDLER->currentEnemyNode = AIHANDLER->currentEnemyNode->GetRandomConnectedNode();
				AIHANDLER->monster->SetNewTarget(AIHANDLER->currentEnemyNode->GetPos());
			}
			else
			{
				AIHANDLER->monster->MoveToTarget(deltaTime);
				//TODO: Add a cooldown timer between state changes
				if (omp_get_wtime() - AIHANDLER->stateSwitchTime > 2.f && AIHANDLER->monster->IsCloseToPlayer(AIHANDLER->player->GetPlayerPos()))
				{
					AIHANDLER->states = EnemyStates::CHASE;
					std::cout << "Switching to Chase\n";
					AIHANDLER->stateSwitchTime = omp_get_wtime();
				}

			}
			break;
		case EnemyStates::CHASE:
			DirectionVec = (AIHANDLER->player->GetPlayerPos() - AIHANDLER->monster->getPosition());
			if ((AIHANDLER->monster->getPosition() - AIHANDLER->player->GetPlayerPos()).Length() < AIHANDLER->monster->GetAttackRange())
			{
				AIHANDLER->monster->PlayAttackAnimation();
			}
			else
			{
				AIHANDLER->monster->Move({ DirectionVec.x, DirectionVec.z }, deltaTime);
			}
			if (omp_get_wtime() - AIHANDLER->stateSwitchTime > 2.f && !AIHANDLER->monster->IsCloseToPlayer(AIHANDLER->player->GetPlayerPos()))
			{
				AIHANDLER->states = EnemyStates::PATROL;
				std::cout << "Switching to Patrol\n";
				AIHANDLER->stateSwitchTime = omp_get_wtime();
				AIHANDLER->monster->SetNewTarget(AIHANDLER->FindClosestNode(AIHANDLER->monster->getPosition())->GetPos());
			}
			break;
		}
	}
	else
	{
	}
}

Node* AIHandler::GetNodeByID(const int& id)
{
	for (int i = 0; i < AIHandler::instance->allNodes.size(); i++)
	{
		if (AIHandler::instance->allNodes.at(i)->GetID() == id)
		{
			return AIHandler::instance->allNodes.at(i);
		}
	}
	return nullptr;
}
