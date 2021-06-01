#include "AIHandler.h"
AIHandler* AIHandler::instance = nullptr;

std::vector<std::string> AIHandler::OpenFile(std::string filePath)
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
	nodeFile.close();

	return allLines;

}
Node* AIHandler::GetRandomNode()
{
	return allNodes.at(rand() % allNodes.size());
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
	this->chaseEnabled = true;
	this->stateSwitchTime = 0.f;
	this->nrOfAstar = 0;
}

const bool AIHandler::Initialize()
{
	if (!AIHANDLER)
	{
		AIHANDLER = new AIHandler;
		AIHANDLER->CreateNodes();
		return true;
	}
	else
	{
		std::cout << "Was already Initialized\n";
		AIHANDLER->CreateNodes();
	}
	return false;
}

AIHandler::~AIHandler()
{
}

void AIHandler::CreateNodes()
{
	if (AIHANDLER->allNodes.empty())
	{
		//Todo: Add correct nodes and add their connections
		std::vector<std::string> file = OpenFile("Resources/Nodes/NodeInfo.txt");//replace with actual name
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
			currentNode->Initialize({ posX, -3.f, posZ }, ID);
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
	}
	AIHANDLER->currentEnemyNode = AIHANDLER->allNodes.at(0);
}

void AIHandler::SetEnemyAndPlayer(Enemy* enemy, Player* player)
{
	AIHANDLER->monster = enemy;
	AIHANDLER->path.clear();
	AIHANDLER->currentEnemyNode = AIHANDLER->FindClosestNode(AIHANDLER->monster->GetPosition());
#ifdef _DEBUG
	std::cout << "Closest Node ID: " << AIHANDLER->currentEnemyNode->GetID() << std::endl;
#endif
	AIHANDLER->monster->SetNewTarget(AIHANDLER->currentEnemyNode->GetPos());
	AIHANDLER->player = player;
}

void AIHandler::ConnectNodes(Node* node1, Node* node2)
{
	node1->AddConnectedNode(node2);
	node2->AddConnectedNode(node1);
}

void AIHandler::MoveEnemy()
{
	if (AIHANDLER->monster)
	{
		switch (AIHANDLER->states)
		{
		case EnemyStates::PATROL:
			AIHANDLER->monster->speed = 10.f;
			if (AIHANDLER->monster->HasReachedTarget())
			{
				if (AIHANDLER->path.size() > 0)
				{
					AIHANDLER->currentEnemyNode = AIHANDLER->path.at(0);
					AIHANDLER->monster->SetNewTarget(AIHANDLER->path.at(0)->GetPos());
					std::cout << "Going to ID: " << AIHANDLER->path.at(0)->GetID() << std::endl;
					AIHANDLER->path.erase(AIHANDLER->path.begin());
				}
				else
				{
					AStarSearch();
				}
			}
			else
			{
				AIHANDLER->monster->RotateTo();
				AIHANDLER->monster->Move();
				std::cout << AIHANDLER->monster->GetPosition().x << " " << AIHANDLER->monster->GetPosition().z << "\r";
				if (AIHANDLER->chaseEnabled && omp_get_wtime() - AIHANDLER->stateSwitchTime > 2.f && AIHANDLER->monster->IsCloseToPlayer(AIHANDLER->player->GetPlayerPos()))
				{
					AIHANDLER->states = EnemyStates::CHASE;
					std::cout << "Switching to Chase\n";
					AIHANDLER->stateSwitchTime = omp_get_wtime();
					AIHANDLER->path.clear();
				}
			}
			break;
		case EnemyStates::CHASE:
			AIHANDLER->monster->speed = 17.5f;
			if (AIHANDLER->chaseEnabled)
			{

				AIHANDLER->monster->SetNewTarget(AIHANDLER->player->GetPlayerPos());
				AIHANDLER->monster->RotateTo();

				if ((AIHANDLER->monster->GetPosition() - AIHANDLER->player->GetPlayerPos()).Length() < AIHANDLER->monster->GetAttackRange())
				{
					if (AIHANDLER->monster->CanAttack())
					{
						AIHANDLER->monster->PlayAttackAnimation();
						AIHANDLER->monster->Attack(AIHANDLER->player);
						//std::cout << "Attacking\n";
					}

				}
				else
				{
					AIHANDLER->monster->Move();
				}
				if (omp_get_wtime() - AIHANDLER->stateSwitchTime > 2.f && !AIHANDLER->monster->IsCloseToPlayer(AIHANDLER->player->GetPlayerPos()))
				{
					AIHANDLER->states = EnemyStates::PATROL;
					std::cout << "Switching to Patrol\n";
					AIHANDLER->stateSwitchTime = omp_get_wtime();
					AIHANDLER->monster->SetNewTarget(AIHANDLER->FindClosestNode(AIHANDLER->monster->GetPosition())->GetPos());
				}
			}
			else
			{
				AIHANDLER->states = EnemyStates::PATROL;
				AIHANDLER->monster->SetNewTarget(AIHANDLER->FindClosestNode(AIHANDLER->monster->GetPosition())->GetPos());
			}
			break;
		}
		AIHANDLER->monster->Update();
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

const bool& AIHandler::IsChaseEnabled()
{
	return AIHANDLER->chaseEnabled;
}

void AIHandler::ToggleChase()
{
	AIHANDLER->chaseEnabled = !AIHANDLER->chaseEnabled;
#ifdef _DEBUG
	if (AIHANDLER->chaseEnabled)
	{
		std::cout << "Chase Enabled\n";
	}
	else
	{
		std::cout << "Chase Disabled\n";
	}
#endif 
}

void AIHandler::ToggleEnemySpeed()
{
	if (AIHANDLER->monster)
	{
		if (AIHANDLER->monster->GetSpeedFactor() == 0.f)
		{
			AIHANDLER->monster->ResetSpeed();
		}
		else
		{
			AIHANDLER->monster->StopMovement();
		}
	}
}

void AIHandler::AStarSearch()
{
	std::vector<Node*> closedList, openList;
	AIHANDLER->path.clear();
	Node* startingNode = AIHANDLER->currentEnemyNode, * goalNode = AIHANDLER->currentEnemyNode;
	openList.push_back(startingNode);
	startingNode->SetFGH(0.f, 0.f, 0.f);
	startingNode->SetParent(startingNode);

	while (goalNode == AIHANDLER->currentEnemyNode)
	{
		goalNode = AIHANDLER->GetRandomNode();
	}

	Node* nodeToAdd = nullptr;
	int index = 0;
	while (!openList.empty() && nodeToAdd != goalNode)
	{
		nodeToAdd = openList.at(0);
		int indexToPop = 0;
		bool stop = false;
		for (unsigned int i = 0; i < openList.size(); i++)
		{
			if (openList.at(i)->GetF() < nodeToAdd->GetF())
			{
				nodeToAdd = openList.at(i);
				indexToPop = i;
			}
		}
		openList.erase(openList.begin() + indexToPop);
		std::vector<Node*> neighbors = nodeToAdd->GetConnectedNodes();

		for (Node* neighbor : neighbors)
		{
			if (neighbor->GetParent() != nodeToAdd && neighbor != nodeToAdd)
			{
				if (!neighbor->GetParent())
				{
					neighbor->SetParent(nodeToAdd);
				}
				if (neighbor == goalNode)
				{
					nodeToAdd = goalNode;
					break;
				}
				if (neighbor->GetF() == FLT_MAX)
				{
					float tempF = 0, tempG = 0, tempH = 0;

					tempG = nodeToAdd->GetG() + (nodeToAdd->GetPos() - neighbor->GetPos()).Length();
					tempH = (goalNode->GetPos() - nodeToAdd->GetPos()).Length(); //Using euclidean distance
					tempF = tempG + tempH;
					neighbor->SetFGH(tempF, tempG, tempH);
				}
				stop = false;
				for (unsigned int i = 0; i < openList.size() && !stop; i++)
				{
					if (openList.at(i)->GetID() == neighbor->GetID())
					{
						stop = true;
					}
				}
				if (closedList.size() > 0)
				{
					for (unsigned int i = 0; i < closedList.size() && !stop; i++)
					{
						if (closedList.at(i)->GetID() == neighbor->GetID())
						{
							stop = true;
						}
					}
					if (!stop)
					{
						openList.push_back(neighbor);
					}
				}
				else
				{
					openList.push_back(neighbor);
				}
			}
		}
		closedList.push_back(nodeToAdd);

		index++;

	}

	AIHANDLER->TracePath(startingNode, goalNode);
	for (unsigned int i = 0; i < AIHANDLER->allNodes.size(); i++)
	{
		AIHANDLER->allNodes.at(i)->ResetFGH();
		AIHANDLER->allNodes.at(i)->ResetParent();
	}
	AIHANDLER->nrOfAstar++;
}
void AIHandler::TracePath(Node* src, Node* dst)
{
	Node* currentNode = dst;
	while (currentNode != src)
	{
		AIHANDLER->path.insert(AIHANDLER->path.begin(), currentNode);
		currentNode = currentNode->GetParent();
	}
}

void AIHandler::Reset()
{
	if (AIHandler::instance)
	{
		AIHandler::instance->currentEnemyNode = nullptr;
		AIHandler::instance->monster = nullptr;
		AIHANDLER->states = EnemyStates::PATROL;
		AIHANDLER->stateSwitchTime = 0.f;
	}
}

void AIHandler::Destroy()
{
	if (AIHANDLER)
	{
		while (!AIHandler::instance->allNodes.empty())
		{
			delete AIHandler::instance->allNodes[(int)AIHandler::instance->allNodes.size() - 1];
			AIHandler::instance->allNodes.pop_back();
		}
		delete AIHANDLER;
	}
}
