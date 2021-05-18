#pragma once
#include "Engine.h"

#define SPOTS 11
class RandomSpotPicker
{
private:

	DirectX::SimpleMath::Vector2 spots[SPOTS];
	int picked[SPOTS];

public:

	RandomSpotPicker();
	virtual ~RandomSpotPicker();

	DirectX::SimpleMath::Vector2 getRandomPos();
	void Reset();
};

