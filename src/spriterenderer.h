#pragma once
#include "texture2d.h"
#include "shader.h"
#include "types.h"

#define glCheckError() glCheckError_(__FILE__, __LINE__) 
GLenum glCheckError_(const char *file, int line);


class SpriteRenderer
{
public:
    SpriteRenderer(ShaderPtr shader);
    ~SpriteRenderer();

    void DrawSprite(Texture2DPtr texture, glm::vec2 position,
        glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f));
private:
    ShaderPtr m_shader;
    GLuint m_quadVAO;

    void initRenderData();
};