#ifndef Panel_hpp
#define Panel_hpp

#include "Define.hpp"
#include "Sprite.hpp"

static constexpr int PANEL_VERTS_COUNT = 4;
static constexpr int NUMBER_OF_PANEL = 3;
static constexpr Vec2f PANEL_TEX_SIZE = { 0.2f, 0.2f };

class Panel : public Sprite<PANEL_VERTS_COUNT>
{
public:
    enum class Type
    {
        BLACK = 0,
        WHITE,
        NONE
    };
    
public:
    Panel();
    Panel(const Vec2f& aSize, const Vec2f& aPos);
    ~Panel();
    
    void SetType(const Type aType);
    Type GetType() const;
    Type GetEnemyType() const;
    
public:
    Type mType;
};

#endif /* Panel_hpp */
