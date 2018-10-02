#pragma once
#include "gameobject.h"
#include "types.h"

#include <glm/glm.hpp>

class BallObject : public GameObject
{
public:
    float m_radius;
    GLboolean m_stuck;

    BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2DPtr sprite);
    ~BallObject() = default;

    glm::vec2 move(GLfloat dt, GLuint window_width);
    void reset(glm::vec2 position, glm::vec2 velocity);

};