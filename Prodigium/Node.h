#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <vector>
class Node
{
private:
	int id;
	DirectX::SimpleMath::Vector3 pos;
	std::vector<Node*> connectedNodes;
public:
	Node();
	virtual ~Node();
	void Initialize(const DirectX::SimpleMath::Vector3& pos, const int& id);
	void AddConnectedNode(Node* node); //Will also add this node to the other one
	const std::vector<Node*>& GetConnectedNodes() const;
	const DirectX::SimpleMath::Vector3& getPos() const;
	const int& getID() const;
};

