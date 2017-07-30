#include "Panel.hpp"

Panel::Panel()
{
}

Panel::Panel(const Vec2f &aSize, const Vec2f &aPos) : Sprite(aSize, aPos)
{
    SetType(Type::NONE);
}

Panel::~Panel()
{
}

void Panel::SetType(const Type aType)
{
    uv[0] = { 1.0f / NUMBER_OF_PANEL * (static_cast<int>(aType) + 0), 1.0f };
    uv[1] = { 1.0f / NUMBER_OF_PANEL * (static_cast<int>(aType) + 1), 1.0f };
    uv[2] = { 1.0f / NUMBER_OF_PANEL * (static_cast<int>(aType) + 1), 0.0f };
    uv[3] = { 1.0f / NUMBER_OF_PANEL * (static_cast<int>(aType) + 0), 0.0f };
    mType = aType;
}

Panel::Type Panel::GetType() const
{
    return mType;
}

Panel::Type Panel::GetEnemyType() const
{
    switch(mType)
    {
        case Panel::Type::BLACK:
            return Panel::Type::WHITE;
        case Panel::Type::WHITE:
            return Panel::Type::BLACK;
        case Panel::Type::NONE:
        default:
            return Panel::Type::NONE;
    }
}
