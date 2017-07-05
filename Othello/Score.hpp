#ifndef Score_hpp
#define Score_hpp

#include <iostream>
#include "Shader.hpp"
#include "Sprite.hpp"
#include "Vec2.hpp"

static constexpr int SCORE_VERTS_COUNT = 4;
static Vec2f SCORE_SIZE = {0.1f, 0.2f};

class Score : public Sprite<SCORE_VERTS_COUNT>
{
public:
    Score(Vec2f aSize, Vec2f aPos);
    
    ~Score();
    
public:
    void Update(int aNum);
    void RefreshUv(int index);
    void RestartUv();
    
private:
    int mNum;
};

#endif /* Score_hpp */
