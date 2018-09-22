#pragma once
#include "glad/glad.h"
#include <memory>

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
    GLuint m_width, m_height;
    class std::shared_ptr<class SpriteRenderer> m_renderer;
    

};