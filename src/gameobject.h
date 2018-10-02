#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

#include "texture2d.h"
#include "spriterenderer.h"

class GameObject
{
public:
    glm::vec2 m_position = glm::vec2(0.f, 0.f);
    glm::vec2 m_size = glm::vec2(1, 1);
    glm::vec2 m_velocity = glm::vec2(0.f);
    glm::vec3 m_color = glm::vec3(1.f);

    GLfloat m_rotation = 0.0f;
    GLboolean m_isSolid = false;
    GLboolean m_isDestroyed = false;

    std::shared_ptr<Texture2D> m_sprite;

public:
    explicit GameObject();
    GameObject(glm::vec2 pos, glm::vec2 size, std::shared_ptr<Texture2D> sprite, glm::vec3 color = glm::vec3(1.0f), glm::vec2 velocity = glm::vec2(0.f, 0.f));
    //Draw Sprite
    virtual void Draw(std::weak_ptr<SpriteRenderer> renderer);
};