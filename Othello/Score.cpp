#include "Score.hpp"

Score::Score(Vec2f aSize, Vec2f aPos)
{
    vertex[0] = geometry[0] = { -aSize.x / 2, +aSize.y / 2 };
    vertex[1] = geometry[1] = { +aSize.x / 2, +aSize.y / 2 };
    vertex[2] = geometry[2] = { +aSize.x / 2, -aSize.y / 2 };
    vertex[3] = geometry[3] = { -aSize.x / 2, -aSize.y / 2 };
    
    RefreshUv(0);
    pos = aPos;
    size = aSize * 0.5f;
    
    for(size_t i = 0; i < 4; i++)
    {
        geometry[i] = pos + vertex[i];
    }
}

Score::~Score()
{
}

void Score::Update(int aNum)
{
    mNum = aNum;
    RefreshUv(aNum);
}

void Score::RefreshUv(int index)
{
    uv[0] = { index / 10.0f, 1 };
    uv[1] = { (index + 1) / 10.0f, 1 };
    uv[2] = { (index + 1) / 10.0f, 0 };
    uv[3] = { index / 10.0f, 0 };
}

void Score::RestartUv()
{
    uv[0] = { 0.0f, 1 };
    uv[1] = { 1 / 10.0f, 1 };
    uv[2] = { 1 / 10.0f, 0 };
    uv[3] = { 0.0f, 0 };
}
