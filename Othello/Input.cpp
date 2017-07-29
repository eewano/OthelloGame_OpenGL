#include "Input.hpp"
#include <iostream>

void Input::Update()
{
    for(size_t i = 0; i < KEY_MAX; i++)
    {
        mKeyStates[i].lastPressed = mKeyStates[i].pressed;
    }
    
    for(size_t i = 0; i < MOUSE_MAX; i++)
    {
        mMouseStates[i].lastPressed = mMouseStates[i].pressed;
    }
}

void Input::ResetNow()
{
    for(size_t i = 0; i < KEY_MAX; i++)
    {
        mKeyStates[i].pressed = false;
    }
    
    for(size_t i = 0; i < MOUSE_MAX; i++)
    {
        mMouseStates[i].pressed = false;
    }
}

bool Input::GetButtonDown(int key)
{
    return mKeyStates[key].lastPressed == false && mKeyStates[key].pressed == true;
}
