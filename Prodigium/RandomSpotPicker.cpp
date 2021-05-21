#include "RandomSpotPicker.h"
#include <iostream>
using namespace DirectX;
RandomSpotPicker::RandomSpotPicker()
{
    // Initialize the random positions over here.
    // X pos is x and Y pos is z
    spots[0] = SimpleMath::Vector2(130.0f, -93.0f);
    spots[1] = SimpleMath::Vector2(-231.0f, -332.0f);
    spots[2] = SimpleMath::Vector2(113, -352);
    spots[3] = SimpleMath::Vector2(401, -176);
    spots[4] = SimpleMath::Vector2(650, 172);
    spots[5] = SimpleMath::Vector2(676, 36);
    spots[6] = SimpleMath::Vector2(163, 148);
    spots[7] = SimpleMath::Vector2(-45, -82);
    spots[8] = SimpleMath::Vector2(-87, 158);
    spots[9] = SimpleMath::Vector2(339, 160);
    spots[10] = SimpleMath::Vector2(325, -58);
    spots[11] = SimpleMath::Vector2(48, 174);
    spots[12] = SimpleMath::Vector2(-275, 84);

    cluesS[0] = "book";
    cluesS[1] = "drawing";
    cluesS[2] = "mask";
    cluesS[3] = "necklace";

    for (int i = 0; i < SPOTS; i++)
    {
        picked[i] = 0;
    }
}

RandomSpotPicker::~RandomSpotPicker()
{
}

DirectX::SimpleMath::Vector2 RandomSpotPicker::getRandomPos()
{
    // Don't let any pickup get same position.
    int random = rand() % SPOTS;
    while(picked[random] == 1)
        random = rand() % SPOTS;

#ifdef _DEBUG
    std::cout << "x: " << spots[random].x << " z:" << spots[random].y << "\n";
#endif
    picked[random] = 1;
    return spots[random];
}

std::string RandomSpotPicker::getRandomClue()
{
    int index = rand() % CLUES_AMOUNT;
    return this->cluesS[index];
}

void RandomSpotPicker::Reset()
{
    for (int i = 0; i < SPOTS; i++)
    {
        picked[i] = 0;
    }
}
