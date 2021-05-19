#include "Node.h"

Node::Node()
{
	this->pos = {};

}

Node::~Node()
{
}

void Node::Initialize(const DirectX::SimpleMath::Vector3& pos, const int& id)
{
	this->pos = pos;
	this->id = id;
}

void Node::AddConnectedNode(Node* node)
{
	this->connectedNodes.push_back(node);
}

const Node* Node::GetRandomConnectedNode() const
{
	int generatedNumber = rand() % this->connectedNodes.size();

	return this->connectedNodes.at(generatedNumber);
}

const std::vector<Node*>& Node::GetConnectedNodes() const
{
	return this->connectedNodes;
}

const DirectX::SimpleMath::Vector3& Node::GetPos() const
{
	return this->pos;
}

const int& Node::GetID() const
{
	return this->id;
}
