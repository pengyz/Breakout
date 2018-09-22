#pragma once
#include "texture2d.h"
#include "shader.h"

#define glCheckError() glCheckError_(__FILE__, __LINE__) 
GLenum glCheckError_(const char *file, int line);


class SpriteRenderer
{
public:
    SpriteRenderer(std::shared_ptr<Shader> shader);
    ~SpriteRenderer();

    void DrawSprite(std::shared_ptr<Texture2D> texture, glm::vec2 position,
        glm::vec2 size = glm::vec2(10, 10), GLfloat rotate = 0.0f,
        glm::vec3 color = glm::vec3(1.0f));
private:
    std::shared_ptr<Shader> m_shader;
    GLuint m_quadVAO;

    void initRenderData();
};