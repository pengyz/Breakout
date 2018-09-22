#pragma once
#include "glad/glad.h"

class Texture2D
{
public:
    GLuint m_id;
    GLuint m_width = 0;
    GLuint m_height = 0;
    GLuint m_internalFormat = GL_RGB;
    GLuint m_imageFormat = GL_RGB;

    GLuint m_wrapS = GL_REPEAT;
    GLuint m_wrapT = GL_REPEAT;
    GLuint m_filterMin = GL_LINEAR;
    GLuint m_filterMax = GL_LINEAR;

public:
    Texture2D();
    ~Texture2D();
    void generate(GLuint width, GLuint height, unsigned char* data);
    void bind() const;
};