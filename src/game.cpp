#include "game.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "resourcemanager.h"
#include "spriterenderer.h"


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
    //������Դ·��
    ResourceManager::get()->addSearchPath("D:/workspace/Breakout");
    // ������ɫ��
    ResourceManager::get()->loadShader(":/resource/sprite.vert", ":/resource/sprite.frag", nullptr, "sprite");
    // ������ɫ��
    glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->m_width),
        static_cast<GLfloat>(this->m_height), 0.0f, -1.0f, 1.0f);
    ResourceManager::get()->getShader("sprite")->use();
    ResourceManager::get()->getShader("sprite")->setUniform("image", 0);
    ResourceManager::get()->getShader("sprite")->setUniform("projection", projection);
    // ����ר������Ⱦ�Ŀ���
    m_renderer = std::make_shared<SpriteRenderer>(ResourceManager::get()->getShader("sprite"));
    // ��������
    ResourceManager::get()->loadTexture(":resource/awesomeface.png", GL_TRUE, "face");
}

//game loops
void Game::ProcessInput(GLfloat dt)
{

}

void Game::Update(GLfloat dt)
{

}

void Game::Render()
{
    m_renderer->DrawSprite(ResourceManager::get()->getTexture("face"), glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}