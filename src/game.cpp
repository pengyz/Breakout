#include "game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

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
    m_ball->move(dt, m_width);
    doCollisions();
}

void Game::Render()
{
    //m_renderer->DrawSprite(ResourceManager::get()->getTexture("face"), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
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
            if (checkCollisionRadius(m_ball, box)) {
                if (!box->m_isSolid) {
                    box->m_isDestroyed = GL_TRUE;
                }
            }
        }
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

bool Game::checkCollisionRadius(BallObjectPtr one, GameObjectPtr two)
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
    return glm::length(difference) < one->m_radius;
}
