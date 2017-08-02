#ifndef Text_hpp
#define Text_hpp

#include <iostream>
#include "Shader.hpp"
#include "Sprite.hpp"
#include "Vec2.hpp"

static constexpr int TEXT_VERTS_COUNT = 4;
static constexpr Vec2f TEXT_BLACK_SIZE = { 0.4f, 0.2f };
static constexpr Vec2f TEXT_WHITE_SIZE = { 0.4f, 0.2f };
static constexpr Vec2f TRIANGLE_SIZE = { 0.2f, 0.2f };

class Text : public Sprite<TEXT_VERTS_COUNT>
{
public:
    Text(Vec2f aSize, Vec2f aPos);
    ~Text();
    
    void TurnChange(char aType);
};

#endif /* Text_hpp */
