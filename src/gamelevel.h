#include "gameobject.h"
#include "spriterenderer.h"

#include <vector>
#include <string>
#include <memory>

class GameLevel
{
public:
    std::vector<std::shared_ptr<GameObject>> m_bricks;

    GameLevel() {}
    GameLevel(const std::string& file, int levelWidth, int levelHeight);
    //���عܿ�
    void load(const std::string& file, int levelWidth, int levelHeight);
    //��Ⱦ�ؿ�
    void Draw(std::weak_ptr<SpriteRenderer> renderer);
    //�Ƿ����
    bool isCompleted();
private:
    void init(std::vector<std::vector<int>> tileData, int levelWidth, int levelHeight);
};