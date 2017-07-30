#ifndef Sprite_h
#define Sprite_h

#include <GLFW/glfw3.h>
#include <iostream>
#include <linmath.h>
#include "Shader.hpp"
#include "Vec2.hpp"
#include "Define.hpp"

extern Shader shader;

template<int VERTS_COUNT>
class Sprite
{
public:
    Sprite()
    {
        std::cout << "Invalid Sprite constructor.\n";
    }
    
    Sprite(const Vec2f& aSize, const Vec2f& aPos)
    : pos(aPos)
    , size(aSize)
    {
        vertex[0] = geometry[0] = { -aSize.x / 2, +aSize.y / 2 };
        vertex[1] = geometry[1] = { +aSize.x / 2, +aSize.y / 2 };
        vertex[2] = geometry[2] = { +aSize.x / 2, -aSize.y / 2 };
        vertex[3] = geometry[3] = { -aSize.x / 2, -aSize.y / 2 };
    }
    
    virtual ~Sprite() = default;
    
public:
    void Draw(GLuint texId)
    {
        for(size_t i = 0; i < VERTS_COUNT; i++)
        {
            geometry[i] = pos + vertex[i];
        }
        
        mat4x4_identity(m);
        mat4x4_translate_in_place(m, pos.x, pos.y, 0);
        mat4x4_ortho(p, -ASPECT_RATIO, ASPECT_RATIO, -1.0f, 1.0f, 1.0f, -1.0f);
        mat4x4_mul(mvp, p, m);
        
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUniformMatrix4fv(shader.mMvpLocation, 1, false, (const GLfloat*)mvp);
        
        glVertexAttribPointer(shader.mPositionLocation, 2, GL_FLOAT, false, 0, geometry);
        glVertexAttribPointer(shader.mUvLocation, 2, GL_FLOAT, false, 0, uv);
        glBindTexture(GL_TEXTURE_2D, texId);
        glDrawArrays(GL_TRIANGLE_FAN, 0, VERTS_COUNT);
    }
    
public:
    Vec2f size{};
    Vec2f pos{};
    Vec2f vertex[VERTS_COUNT]{};
    Vec2f geometry[VERTS_COUNT]{};
    Vec2f uv[VERTS_COUNT]{};
    
private:
    mat4x4 m, p, mvp;
};

#endif /* Sprite_h */
