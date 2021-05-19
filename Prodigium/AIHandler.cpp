#include "AIHandler.h"
AIHandler* AIHandler::instance = nullptr;

AIHandler::AIHandler()
{
}

const bool AIHandler::Initialize()
{
	if (!AIHandler::instance)
	{
		AIHandler::instance = new AIHandler;
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

void AIHandler::CreateNodeGrid()
{
	for (int i = 0; i < 20; i++)
	{
		Node* currentNode = new Node();
		AIHandler::instance->allNodes.push_back(currentNode);
	}
	AIHandler::instance->allNodes.at(0)->Initialize({ 0.f,0.f,0.f }, 0);
	AIHandler::instance->allNodes.at(1)->Initialize({ 0.f,0.f,0.f }, 0);
	AIHandler::instance->allNodes.at(2)->Initialize({ 0.f,0.f,0.f }, 0);
	AIHandler::instance->allNodes.at(3)->Initialize({ 0.f,0.f,0.f }, 0);
	AIHandler::instance->allNodes.at(4)->Initialize({ 0.f,0.f,0.f }, 0);
	AIHandler::instance->allNodes.at(5)->Initialize({ 0.f,0.f,0.f }, 0);
	AIHandler::instance->allNodes.at(6)->Initialize({ 0.f,0.f,0.f }, 0);

}
