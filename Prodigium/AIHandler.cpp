#include "AIHandler.h"
AIHandler* AIHandler::instance = nullptr;

std::vector<std::string> AIHandler::openFile(std::string filePath)
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

AIHandler::AIHandler()
{
}

const bool AIHandler::Initialize()
{
	if (!AIHandler::instance)
	{
		AIHandler::instance = new AIHandler;
		return true;
	}
	return false;
}

AIHandler::~AIHandler()
{
	if (AIHandler::instance)
	{
		delete AIHandler::instance;
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
		AIHandler::instance->allNodes.push_back(currentNode);
		currentIndex++;
	}
	std::cout << "Connecting Nodes\n";
	for (int i = currentIndex + 1; i < file.size(); i++)
	{
		std::stringstream ss(file.at(i));
		int ID1 = 0, ID2 = 0;
		ss >> ID1 >> ID2;
		AIHandler::instance->ConnectNodes(AIHandler::instance->GetNodeByID(ID1), AIHandler::instance->GetNodeByID(ID2));
	}
	//Connecting Nodes;
}

void AIHandler::ConnectNodes(Node* node1, Node* node2)
{
	node1->AddConnectedNode(node2);
	node2->AddConnectedNode(node1);
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
