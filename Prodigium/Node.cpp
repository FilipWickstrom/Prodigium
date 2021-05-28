#include "Node.h"

Node::Node()
{
	this->pos = {};
	this->f = FLT_MAX;
	this->g = FLT_MAX;
	this->h = FLT_MAX;
	this->id = -1;
	this->parent = nullptr;
}

Node::~Node()
{
	this->connectedNodes.clear();
	this->parent = nullptr;
}

void Node::Initialize(const DirectX::SimpleMath::Vector3& pos, const int& id)
{
	this->pos = pos;
	this->id = id;
}

void Node::AddConnectedNode(Node* node)
{
	bool found = false;
	for (int i = 0; i < connectedNodes.size() && !found; i++)
	{
		if (connectedNodes.at(i)->GetID() == node->GetID() || node->GetID() == this->id)
		{
			found = true;
		}
	}
	if (!found)
	{
		this->connectedNodes.push_back(node);
	}
}

Node* Node::GetRandomConnectedNode() const
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

void Node::SetFGH(const float& f, const float& g, const float& h)
{
	this->f = f;
	this->g = g;
	this->h = h;
}

void Node::ResetFGH()
{
	this->f = FLT_MAX;
	this->g = FLT_MAX;
	this->h = FLT_MAX;
}

void Node::ResetParent()
{
	this->parent = nullptr;
}

void Node::SetF(const float& f)
{
	this->f = f;
}

void Node::SetG(const float& g)
{
	this->g = g;
}

void Node::SetH(const float& h)
{
	this->h = h;
}

void Node::SetParent(Node* parent)
{
	this->parent = parent;
}

Node* Node::GetParent() const
{
	return this->parent;
}

const float& Node::GetF() const
{
	return this->f;
}

const float& Node::GetG() const
{
	return this->g;
}
