#include "gameobject.h"
#include <memory>

GameObject::GameObject() {}

GameObject::GameObject(glm::vec2 pos, glm::vec2 size, std::shared_ptr<Texture2D> sprite, glm::vec3 color, glm::vec2 velocity) :
    m_position(pos), m_size(size), m_velocity(velocity), m_color(color), m_rotation(0.0f), m_sprite(sprite),
    m_isSolid(false), m_isDestroyed(false)
{ }

void GameObject::Draw(std::weak_ptr<SpriteRenderer> renderer)
{
    auto pRenderer = renderer.lock();
    if (pRenderer)
        pRenderer->DrawSprite(this->m_sprite, this->m_position, this->m_size, this->m_rotation, this->m_color);
}