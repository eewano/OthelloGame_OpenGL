#ifndef Panel_hpp
#define Panel_hpp

#include <iostream>
#include "Define.hpp"
#include "Shader.hpp"
#include "Sprite.hpp"
#include "Vec2.hpp"

static constexpr int STONE_VERTS_COUNT = 4;
static Vec2f STONE_SIZE = { 0.2f, 0.2f };

class Panel : public Sprite<4>
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
    Panel(Vec2f aSize, Vec2f aPos);
    ~Panel();
    
    void Update(Type aType);
    void RefreshUv(Type aType);
    Type GetType() const;
    
public:
    Type mType;
};

#endif /* Panel_hpp */
