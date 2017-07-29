#ifndef Input_hpp
#define Input_hpp

#include "Vec2.hpp"

struct State
{
    bool pressed;
    bool lastPressed;
};

class Input
{
public:
    static constexpr int KEY_MAX = 512;
    static constexpr int MOUSE_MAX = 16;
    
public:
    void Update();
    void ResetNow();
    bool GetButtonDown(int key);
    
public:
    State mKeyStates[KEY_MAX];
    State mMouseStates[MOUSE_MAX];
    Vec2i mCursorPosition;
};

#endif /* Input_hpp */
