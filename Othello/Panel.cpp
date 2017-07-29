#include "Panel.hpp"

Panel::Panel()
{
}

Panel::Panel(Vec2f aSize, Vec2f aPos)
{
    vertex[0] = geometry[0] = { -aSize.x / 2, +aSize.y / 2 };
    vertex[1] = geometry[1] = { +aSize.x / 2, +aSize.y / 2 };
    vertex[2] = geometry[2] = { +aSize.x / 2, -aSize.y / 2 };
    vertex[3] = geometry[3] = { -aSize.x / 2, -aSize.y / 2 };
    
    mType = Type::NONE;
    RefreshUv(mType);
    pos = aPos;
    size = aSize;
}

Panel::~Panel()
{
}

void Panel::Update(Type aType)
{
    mType = aType;
    RefreshUv(mType);
}

void Panel::RefreshUv(Type aType)
{
    uv[0] = { (0.0f + (int)aType) / 3.0f, 1.0f };
    uv[1] = { (1.0f + (int)aType) / 3.0f, 1.0f };
    uv[2] = { (1.0f + (int)aType) / 3.0f, 0.0f };
    uv[3] = { (0.0f + (int)aType) / 3.0f, 0.0f };
}

Panel::Type Panel::GetType() const
{
    return mType;
}
