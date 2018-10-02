#include "ballobject.h"

BallObject::BallObject(glm::vec2 pos, GLfloat radius, glm::vec2 velocity, Texture2DPtr sprite)
    :GameObject(pos, glm::vec2(radius * 2, radius * 2), sprite, glm::vec3(1.f), velocity)
    , m_radius(radius), m_stuck(true)
{

}

glm::vec2 BallObject::move(GLfloat dt, GLuint window_width)
{
    if (!m_stuck) {
        //move ball
        m_position += dt * m_velocity;
        //bound check
        if (m_position.x <= 0.f) {
            m_velocity.x = -m_velocity.x;
            m_position.x = 0;
        } else if (m_position.x + m_size.x >= window_width) {
            m_velocity.x = -m_velocity.x;
            m_position.x = window_width - m_size.x;
        } else if (m_position.y <= 0) {
            m_velocity.y = -m_velocity.y;
            m_position.y = 0;
        }
    }
    return m_position;
}

void BallObject::reset(glm::vec2 position, glm::vec2 velocity)
{
    m_position = position;
    m_velocity = velocity;
    m_stuck = true;
}

