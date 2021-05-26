#pragma once
#include "Engine.h"

#define SPOTS 13
#define CLUES_AMOUNT 6
class RandomSpotPicker
{
private:

	DirectX::SimpleMath::Vector2 spots[SPOTS];
	int picked[SPOTS];
	std::string cluesS[CLUES_AMOUNT];

public:

	RandomSpotPicker();
	virtual ~RandomSpotPicker();

	DirectX::SimpleMath::Vector2 GetRandomPos();
	std::string GetRandomClue();
	void Reset();
};

