#pragma once
#include "Engine.h"

class NodeFinder
{
private:

	DirectX::SimpleMath::Vector3 currentPos;
	DirectX::SimpleMath::Vector3 goalPos;
	DirectX::SimpleMath::Vector3 moveToPos;
	std::vector<DirectX::SimpleMath::Vector3> nodes;
	int index;

public:

	NodeFinder();
	virtual ~NodeFinder();
	float distance(DirectX::SimpleMath::Vector3 otherNode);

	DirectX::SimpleMath::Vector3 MoveTo();
	void Update(DirectX::SimpleMath::Vector3 goalPos, DirectX::SimpleMath::Vector3 currentPos);
};