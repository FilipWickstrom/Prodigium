#pragma once
#include <d3d11.h>
#include <SimpleMath.h>
#include <vector>
class Node
{
private:
	int id;
	float f,g,h;
	DirectX::SimpleMath::Vector3 pos;
	std::vector<Node*> connectedNodes;
	Node* parent;
public:
	Node();
	virtual ~Node();
	void Initialize(const DirectX::SimpleMath::Vector3& pos, const int& id);
	void AddConnectedNode(Node* node);
	Node* GetRandomConnectedNode() const;
	const std::vector<Node*>& GetConnectedNodes() const;
	const DirectX::SimpleMath::Vector3& GetPos() const;
	const int& GetID() const;
	void SetFGH(const float& f, const float& g, const float& h);
	void ResetFGH();
	void ResetParent();
	void SetF(const float& f);
	void SetG(const float& g);
	void SetH(const float& h);
	void SetParent(Node* parent);
	Node* GetParent() const;
	const float& GetF() const;
	const float& GetG() const;
};

