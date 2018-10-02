#include "gamelevel.h"
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>

#include "types.h"
#include "resourcemanager.h"

GameLevel::GameLevel(const std::string& file, int levelWidth, int levelHeight)
{
    load(file, levelWidth, levelHeight);
}


void GameLevel::load(const std::string& file, int levelWidth, int levelHeight)
{
    m_bricks.clear();

    int tileCode;
    GameLevel leve;
    std::string line;
    std::ifstream fstream(file);
    std::vector<std::vector<int>> tileData;
    if (fstream) {
        while (std::getline(fstream, line)) {
            std::istringstream sstream(line);
            std::vector<int> row;
            while (sstream >> tileCode)
                row.push_back(tileCode);
            tileData.push_back(row);
        }
        if (tileData.size() > 0)
            init(tileData, levelWidth, levelHeight);
    }
}

void GameLevel::init(std::vector<std::vector<int>> tileData, int levelWidth, int levelHeight)
{
    GLuint height = tileData.size();
    GLuint width = tileData[0].size();
    GLfloat unit_width = (GLfloat)((GLfloat)levelWidth / width);
    GLfloat unit_height = (GLfloat)((GLfloat)levelHeight / height);

    for (GLuint y = 0; y < height; y++) {
        for (GLuint x = 0; x < width; x++) {
            //check brick type
            if (tileData[y][x] == 1) {
                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                GameObjectPtr obj = std::make_shared<GameObject>(pos, size, ResourceManager::get()->getTexture("block_solid"), glm::vec3(0.8f, 0.8f, 0.7f));
                obj->m_isSolid = true;
                m_bricks.push_back(obj);
            } else if (tileData[y][x] > 1) {
                glm::vec3 color(1.f);
                if (tileData[y][x] == 2) {
                    color = glm::vec3(0.2f, 0.6f, 1.0f);
                } else if (tileData[y][x] == 3) {
                    color = glm::vec3(0.f, 0.7f, 0.f);
                } else if (tileData[y][x] == 4) {
                    color = glm::vec3(0.8f, 0.8f, 0.4f);
                } else if (tileData[y][x] == 5) {
                    color = glm::vec3(1.0f, 0.5f, 0.0f);
                }

                glm::vec2 pos(unit_width * x, unit_height * y);
                glm::vec2 size(unit_width, unit_height);
                m_bricks.push_back(std::make_shared<GameObject>(pos, size, ResourceManager::get()->getTexture("block"), color));
            }
        }
    }
}

void GameLevel::Draw(std::weak_ptr<SpriteRenderer> renderer)
{
    for (auto& tile : m_bricks) {
        if (!tile->m_isDestroyed) {
            tile->Draw(renderer);
        }
    }
}

bool GameLevel::isCompleted()
{
    for (auto& tile : m_bricks) {
        if (!tile->m_isSolid && !tile->m_isDestroyed) {
            return false;
        }
    }
    return true;
}