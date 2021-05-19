#include "NodeFinder.h"

NodeFinder::NodeFinder()
{
    this->currentPos = { 0.0f, 0.0f, 0.0f };
    this->goalPos = { 0.0f, 0.0f, 0.0f };
    this->index = -1;

    float y_value = 0.0f;
    // Setup nodes
    nodes.push_back({ 50.0f, y_value, -100.0f });
    nodes.push_back({ 100.0f, y_value, -150.0f });
    nodes.push_back({ 50.0f, y_value, 40.0f });
}

NodeFinder::~NodeFinder()
{
}

float NodeFinder::distance(DirectX::SimpleMath::Vector3 otherNode)
{
    float dist = (this->currentPos - otherNode).Length();

    if (dist < 0)
        dist *= -1;

    return dist;
}

DirectX::SimpleMath::Vector3 NodeFinder::MoveTo()
{
    //std::cout << "x:" << this->moveToPos.x << " y:" << this->moveToPos.y << " z:" << this->moveToPos.z << "\n";
    return this->moveToPos;
}

void NodeFinder::Update(DirectX::SimpleMath::Vector3 goalPos, DirectX::SimpleMath::Vector3 currentPos)
{
    this->goalPos = goalPos;
    this->currentPos = currentPos;

    float length = 999999;
    for (int i = 0; i < (int)nodes.size(); i++)
    {
        if (i != this->index && (this->goalPos - this->nodes[i]).Length() < length)
        {
            length = (this->goalPos - this->nodes[i]).Length();
            std::cout << length << "\n";
            this->moveToPos = nodes[i];
            this->index = i;

            if ((this->currentPos - this->goalPos).Length() < (this->currentPos - this->nodes[i]).Length())
                this->moveToPos = this->goalPos;
        }
    }
}
