#ifndef Shader_hpp
#define Shader_hpp

#include <GLFW/glfw3.h>

class Shader
{
public:
    int mPositionLocation;
    int mUvLocation;
    int mTextureLocation;
    int mMvpLocation;
    
    static const char* ID_position;
    static const char* ID_uv;
    static const char* ID_texture;
    static const char* ID_MVP;
    
public:
    Shader();
    ~Shader();
    
    void SetUp();
    
private:
    GLuint mProgramId;
};

#endif /* Shader_hpp */
