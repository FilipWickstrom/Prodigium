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
	node->AddConnectedNode(this);
}

const std::vector<Node*>& Node::GetConnectedNodes() const
{
	return this->connectedNodes;
}

const DirectX::SimpleMath::Vector3& Node::getPos() const
{
	return this->pos;
}

const int& Node::getID() const
{
	return this->id;
}
