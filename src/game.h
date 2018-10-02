#pragma once
#include "glad/glad.h"
#include "types.h"

#include <vector>
#include <glm/glm.hpp>

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN,
};

class Game
{
public:
    Game(GLuint width, GLuint height);
    ~Game();
    //initialize game
    void Init();
    //game loops
    void ProcessInput(GLfloat dt);
    void Update(GLfloat dt);
    void Render();


public:
    GameState getState() { return m_state; }
    void setState(GameState state) { m_state = state; }
    void setKey(int keyId, bool isPressed) { m_keys[keyId] = isPressed; }

private:
    GameState m_state;
    GLboolean m_keys[1024];
    GLuint m_width;
    GLuint m_height;
    SpriteRendererPtr m_renderer = nullptr;
    std::vector<GameLevelPtr> m_levels;
    int m_currLevel = -1;
    BallObjectPtr m_ball = nullptr;

    const glm::vec2 PLAYER_SIZE = glm::vec2(100, 20);
    const GLfloat PLAYER_VELOCITY = 500.0f;
    GameObjectPtr m_player = nullptr;

    //ball const values
    const glm::vec2 INITIAL_BALL_VELOCITY = glm::vec2(100.0f, -350.0f);
    const GLfloat BALL_RADIUS = 12.5f;
};