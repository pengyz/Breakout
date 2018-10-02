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