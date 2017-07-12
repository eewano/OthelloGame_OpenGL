#include "Shader.hpp"
#include <iostream>

const char* Shader::ID_position = "position";
const char* Shader::ID_uv = "uv";
const char* Shader::ID_texture = "texture";
const char* Shader::ID_MVP = "MVP";

Shader::Shader()
{
}

Shader::~Shader()
{
}

void Shader::SetUp()
{
    auto vShaderId = glCreateShader(GL_VERTEX_SHADER);
    std::string vertexShader = R"#(
    uniform mat4 MVP;
    attribute vec3 position;
    attribute vec2 uv;
    varying vec2 vuv;
    void main(void){
        gl_Position = MVP * vec4(position, 1.0);
        vuv = uv;
    }
    )#";
    const char* vs = vertexShader.c_str();
    glShaderSource(vShaderId, 1, &vs, NULL);
    glCompileShader(vShaderId);
    
    GLuint fShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    std::string fragmentShader = R"#(
    varying vec2 vuv;
    uniform sampler2D texture;
    void main(void){
        gl_FragColor = texture2D(texture, vuv);
    }
    )#";
    const char* fs = fragmentShader.c_str();
    glShaderSource(fShaderId, 1, &fs, NULL);
    glCompileShader(fShaderId);
    
    GLuint programId = glCreateProgram();
    glAttachShader(programId, vShaderId);
    glAttachShader(programId, fShaderId);
    
    glLinkProgram(programId);
    
    glUseProgram(programId);
    
    mProgramId = programId;
    
    mPositionLocation = glGetAttribLocation(programId, Shader::ID_position);
    if(mPositionLocation == -1)
    {
        std::cerr << "glGetAttribLocation failed " << Shader::ID_position << ".\n";
    }
    mUvLocation = glGetAttribLocation(programId, Shader::ID_uv);
    if(mUvLocation == -1)
    {
        std::cerr << "glGetAttribLocation failed " << Shader::ID_uv << ".\n";
    }
    mTextureLocation = glGetUniformLocation(programId, Shader::ID_texture);
    if(mTextureLocation == -1)
    {
        std::cerr << "glGetUniformLocation failed " << Shader::ID_texture << ".\n";
    }
    mMvpLocation = glGetUniformLocation(programId, Shader::ID_MVP);
    if(mMvpLocation == -1)
    {
        std::cerr << "glGetUniformLocation failed " << Shader::ID_MVP << ".\n";
    }
    
    glEnableVertexAttribArray(mPositionLocation);
    glEnableVertexAttribArray(mUvLocation);
    glEnableVertexAttribArray(mMvpLocation);
    
    glUniform1i(mTextureLocation, 0);
}
