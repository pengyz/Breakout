#include "game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>
#include <tuple>
#include <algorithm>

#include "resourcemanager.h"
#include "spriterenderer.h"
#include "gamelevel.h"
#include "spriterenderer.h"
#include "ballobject.h"


Game::Game(GLuint width, GLuint height)
    :m_width(width), m_height(height)
{
}

Game::~Game()
{

}

//initialize game
void Game::Init()
{
    //设置资源路径
    ResourceManager::get()->addSearchPath("D:/workspace/Breakout");
    // 加载着色器
    ResourceManager::get()->loadShader(":/resource/shaders/sprite.vert", ":/resource/shaders/sprite.frag", nullptr, "sprite");
    // 配置着色器
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->m_width),
        static_cast<GLfloat>(this->m_height), 0.0f, -1.0f, 1.0f);
    ResourceManager::get()->getShader("sprite")->use();
    ResourceManager::get()->getShader("sprite")->setUniform("image", 0);
    ResourceManager::get()->getShader("sprite")->setUniform("projection", projection);
    // 设置专用于渲染的控制
    m_renderer = std::make_shared<SpriteRenderer>(ResourceManager::get()->getShader("sprite"));

    // 加载纹理
    ResourceManager::get()->loadTexture(":/resource/textures/awesomeface.png", GL_TRUE, "face");
    ResourceManager::get()->loadTexture(":/resource/textures/background.jpg", GL_FALSE, "background");
    ResourceManager::get()->loadTexture(":/resource/textures/block.png", GL_FALSE, "block");
    ResourceManager::get()->loadTexture(":/resource/textures/block_solid.png", GL_FALSE, "block_solid");
    ResourceManager::get()->loadTexture(":/resource/textures/paddle.png", GL_TRUE, "paddle");
    //加载关卡
    m_levels.emplace_back(std::make_shared<GameLevel>(ResourceManager::get()->solveResourcePath(":/resource/levels/one.lvl"), m_width, m_height * 0.5));
    m_levels.emplace_back(std::make_shared<GameLevel>(ResourceManager::get()->solveResourcePath(":/resource/levels/two.lvl"), m_width, m_height * 0.5));
    m_levels.emplace_back(std::make_shared<GameLevel>(ResourceManager::get()->solveResourcePath(":/resource/levels/three.lvl"), m_width, m_height * 0.5));
    m_levels.emplace_back(std::make_shared<GameLevel>(ResourceManager::get()->solveResourcePath(":/resource/levels/four.lvl"), m_width, m_height * 0.5));
    m_currLevel = 0;
    //初始化档板
    glm::vec2 playerPos = glm::vec2(this->m_width / 2 - PLAYER_SIZE.x / 2, this->m_height - PLAYER_SIZE.y);
    m_player = std::make_shared<GameObject>(playerPos, PLAYER_SIZE, ResourceManager::get()->getTexture("paddle"));
    //初始化球
    glm::vec2 ballPos = playerPos + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    m_ball = std::make_shared<BallObject>(ballPos, BALL_RADIUS, INITIAL_BALL_VELOCITY, ResourceManager::get()->getTexture("face"));
}

//game loops
void Game::ProcessInput(GLfloat dt)
{
    if (this->m_state == GAME_ACTIVE) {
        GLfloat velocity = PLAYER_VELOCITY * dt;
        // 移动挡板
        if (this->m_keys[GLFW_KEY_A]) {
            if (m_player->m_position.x >= 0)
                m_player->m_position.x -= velocity;
            if (m_ball->m_stuck)
                m_ball->m_position.x -= velocity;
        }
        if (this->m_keys[GLFW_KEY_D]) {
            if (m_player->m_position.x <= this->m_width - m_player->m_size.x)
                m_player->m_position.x += velocity;
            if (m_ball->m_stuck)
                m_ball->m_position.x += velocity;
        }
        if (m_keys[GLFW_KEY_SPACE]) {
            m_ball->m_stuck = false;
        }
    }
}

void Game::Update(GLfloat dt)
{
    doCollisions();
    m_ball->move(dt, m_width);
}

void Game::Render()
{
    if (m_state == GAME_ACTIVE) {
        m_renderer->DrawSprite(ResourceManager::get()->getTexture("background"), glm::vec2(0.f, 0.f), glm::vec2(m_width, m_height), 0.f);
        m_levels[m_currLevel]->Draw(m_renderer);
        m_player->Draw(m_renderer);
        m_ball->Draw(m_renderer);
    }
}

void Game::doCollisions()
{
    for (auto& box : m_levels[m_currLevel]->m_bricks) {
        if (!box->m_isDestroyed) {
            auto collision = checkCollisionRadius(m_ball, box);
            if (std::get<0>(collision)) {
                if (!box->m_isSolid) {
                    box->m_isDestroyed = GL_TRUE;
                }
                auto dir = std::get<1>(collision);
                auto diffVec = std::get<2>(collision);
                if (dir == LEFT || dir == RIGHT) {
                    m_ball->m_velocity.x = -m_ball->m_velocity.x;
                    auto pentration = m_ball->m_radius - std::abs(diffVec.x);
                    if (dir == LEFT) {
                        m_ball->m_position.x += pentration;
                    } else {
                        m_ball->m_position.x -= pentration;
                    }
                } else {
                    m_ball->m_velocity.y = -m_ball->m_velocity.y;
                    auto pentration = m_ball->m_radius - std::abs(diffVec.y);
                    if (dir == UP) {
                        m_ball->m_position.y -= pentration;
                    } else {
                        m_ball->m_position.y += pentration;
                    }
                }
                //break;
            }
        }
    }
    //check paddle collision
    Collision result = checkCollisionRadius(m_ball, m_player);
    if (!m_ball->m_stuck && std::get<0>(result)) {
        // 检查碰到了挡板的哪个位置，并根据碰到哪个位置来改变速度
        GLfloat centerBoard = m_player->m_position.x + m_player->m_size.x / 2;
        GLfloat distance = (m_ball->m_position.x + m_ball->m_radius) - centerBoard;
        GLfloat percentage = distance / (m_player->m_size.x / 2);
        // 依据结果移动
        GLfloat strength = 2.0f;
        glm::vec2 oldVelocity = m_ball->m_velocity;
        m_ball->m_velocity.x = INITIAL_BALL_VELOCITY.x * percentage * strength;
        m_ball->m_velocity.y = -1 * std::abs(m_ball->m_velocity.y);
        m_ball->m_velocity = glm::normalize(m_ball->m_velocity) * glm::length(oldVelocity);
    }
    //check ball fallen
    if (m_ball->m_position.y > m_height) {
        resetLevel();
        resetPlayer();
    }
}

bool Game::checkCollisionAABB(GameObjectPtr first, GameObjectPtr second)
{
    bool isCollisionX = first->m_position.x + first->m_size.x >= second->m_position.x &&
        first->m_position.x < second->m_position.x + second->m_size.x;
    bool isCollisionY = first->m_position.y + first->m_size.y >= second->m_position.y &&
        first->m_position.y < second->m_position.y + second->m_size.y;
    return isCollisionX && isCollisionY;
}

Collision Game::checkCollisionRadius(BallObjectPtr one, GameObjectPtr two)
{
    // 获取圆的中心 
    glm::vec2 center(one->m_position + one->m_radius);
    // 计算AABB的信息（中心、半边长）
    glm::vec2 aabb_half_extents(two->m_size.x / 2, two->m_size.y / 2);
    glm::vec2 aabb_center(two->m_position.x + aabb_half_extents.x, two->m_position.y + aabb_half_extents.y);
    // 获取两个中心的差矢量
    glm::vec2 difference = center - aabb_center;
    glm::vec2 clamped = glm::clamp(difference, -aabb_half_extents, aabb_half_extents);
    // AABB_center加上clamped这样就得到了碰撞箱上距离圆最近的点closest
    glm::vec2 closest = aabb_center + clamped;
    // 获得圆心center和最近点closest的矢量并判断是否 length <= radius
    difference = closest - center;
    bool bCollision = glm::length(difference) <= one->m_radius;
    return std::make_tuple(bCollision ? GL_TRUE : GL_FALSE, bCollision ? VectorDirection(difference) : UP, bCollision ? difference : glm::vec2(0.f, 0.f));
}

Direction Game::VectorDirection(glm::vec2 target)
{
    glm::vec2 compass[] = {
        glm::vec2(0.0f, 1.0f),  // 上
        glm::vec2(1.0f, 0.0f),  // 右
        glm::vec2(0.0f, -1.0f), // 下
        glm::vec2(-1.0f, 0.0f)  // 左
    };
    GLfloat max = 0.0f;
    GLuint best_match = -1;
    for (GLuint i = 0; i < 4; i++) {
        GLfloat dot_product = glm::dot(glm::normalize(target), compass[i]);
        if (dot_product > max) {
            max = dot_product;
            best_match = i;
        }
    }
    return (Direction)best_match;
}

void Game::resetLevel()
{
    m_levels[m_currLevel]->reset();
}

void Game::resetPlayer()
{
    m_player->m_position = glm::vec2(this->m_width / 2 - PLAYER_SIZE.x / 2, this->m_height - PLAYER_SIZE.y);
    m_ball->m_position = m_player->m_position + glm::vec2(PLAYER_SIZE.x / 2 - BALL_RADIUS, -BALL_RADIUS * 2);
    m_ball->m_stuck = true;
}