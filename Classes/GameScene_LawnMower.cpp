#include "GameScene.h"
#include "Zombie.h"
#include <iostream>

USING_NS_CC;

// 初始化推车系统
void GameScene::initLawnMowerSystem() {
    // 初始化推车数组，每行一个推车
    for (int i = 0; i < GRID_ROWS; i++) {
        lawnMowers[i] = nullptr;
        lawnMowerActive[i] = false;
    }

    // 创建每行的推车
    for (int row = 0; row < GRID_ROWS; row++) {
        createLawnMowerForRow(row);
    }
}
// 为指定行创建推车
void GameScene::createLawnMowerForRow(int row) {
    // 如果该行已经有推车，则不重复创建
    if (lawnMowers[row] != nullptr) {
        return;
    }

    // 创建推车精灵
    auto lawnMower = Sprite::create("LawnMower.png");
   
    // 设置推车位置（每行的最左侧，网格之外）
    Vec2 gridCenter = getGridCenter(row, 0);
    float xPos = bgScreenStartX + 350; // 屏幕左侧边缘
    float yPos = gridCenter.y;

    // 调整推车大小以适应网格
    float lawnMowerScale = cellHeight / lawnMower->getContentSize().height * 0.8f;
    lawnMower->setScale(lawnMowerScale);
    lawnMower->setPosition(Vec2(xPos, yPos));

    // 保存推车引用
    lawnMowers[row] = lawnMower;
    lawnMowerActive[row] = false;

    // 添加到场景中（在僵尸下方，植物上方）
    this->addChild(lawnMower, 6);
}
// 激活指定行的推车
void GameScene::activateLawnMower(int row) {
    // 检查行号是否有效
    if (row < 0 || row >= GRID_ROWS) {
        return;
    }

    // 检查推车是否存在且未激活
    if (lawnMowers[row] == nullptr || lawnMowerActive[row]) {
        return;
    }

    // 标记推车为激活状态
    lawnMowerActive[row] = true;

    // 获取推车精灵
    auto lawnMower = lawnMowers[row];
    // 创建推车启动动画
    auto moveTo = MoveTo::create(3.0f, Vec2(bgScreenEndX + 100, lawnMower->getPositionY()));
    auto easeOut = EaseOut::create(moveTo, 2.0f);
    // 启动动画序列
    lawnMower->runAction(Sequence::create(
        easeOut,
        CallFunc::create([this, row]() {
            // 动画完成后移除推车
            this->removeLawnMower(row);
            }),
        nullptr
    ));

    // 消除该行的所有僵尸
    clearZombiesInRow(row);
}
// 清除指定行的所有僵尸
void GameScene::clearZombiesInRow(int row) {
    Vector<Zombie*> zombiesToRemove;

    // 收集该行的所有僵尸
    for (auto& zombie : zombies) {
        if (zombie && zombie->getHP() > 0) {
            // 获取僵尸的网格位置（近似判断）
            int zombieRow = getZombieRow(zombie);
            if (zombieRow == row) {
                zombiesToRemove.pushBack(zombie);
            }
        }
    }

    // 移除该行的所有僵尸
    for (auto& zombie : zombiesToRemove) {
        if (zombie) {
            // 设置僵尸死亡
            zombie->setHP(0);

            // 播放僵尸被碾压的动画
            auto fadeOut = FadeOut::create(0.2f);
            auto scaleDown = ScaleTo::create(0.2f, 0.1f);
            auto remove = RemoveSelf::create();
            auto sequence = Sequence::create(
                Spawn::create(fadeOut, scaleDown, nullptr),
                remove,
                nullptr
            );
            zombie->runAction(sequence);

            // 从数组中移除
            zombies.eraseObject(zombie);
        }
    }
}
// 获取僵尸所在的行（根据Y坐标判断）
int GameScene::getZombieRow(Zombie* zombie) {
    if (!zombie) 
        return -1;

    float zombieY = zombie->getPositionY();

    // 检查僵尸在哪一行
    for (int row = 0; row < GRID_ROWS; row++) {
        float rowTop = gridStartY + row * cellHeight + cellHeight;
        float rowBottom = gridStartY + row * cellHeight;

        if (zombieY >= rowBottom && zombieY <= rowTop) {
            return row;
        }
    }

    return -1;
}
// 检查僵尸和推车的碰撞
void GameScene::checkLawnMowerCollision() {
    for (int row = 0; row < GRID_ROWS; row++) {
        // 如果推车已经激活或不存在，跳过
        if (lawnMowerActive[row] || lawnMowers[row] == nullptr) {
            continue;
        }

        auto lawnMower = lawnMowers[row];

        // 获取推车的位置和大小
        Vec2 lawnMowerPos = lawnMower->getPosition();
        float lawnMowerWidth = lawnMower->getContentSize().width * lawnMower->getScale() * 0.8f;
        float lawnMowerHeight = lawnMower->getContentSize().height * lawnMower->getScale() * 0.8f;

        Rect lawnMowerRect = Rect(
            lawnMowerPos.x - lawnMowerWidth / 2,
            lawnMowerPos.y - lawnMowerHeight / 2,
            lawnMowerWidth,
            lawnMowerHeight
        );

        // 检查该行的每个僵尸
        for (auto& zombie : zombies) {
            if (!zombie || zombie->getHP() <= 0) continue;

            // 检查僵尸是否在推车所在的行
            int zombieRow = getZombieRow(zombie);
            if (zombieRow != row) continue;

            // 获取僵尸的位置和大小
            Vec2 zombiePos = zombie->getPosition();
            float zombieWidth = zombie->getContentSize().width * zombie->getScale() * 0.5f;
            float zombieHeight = zombie->getContentSize().height * zombie->getScale() * 0.5f;

            Rect zombieRect = Rect(
                zombiePos.x - zombieWidth,
                zombiePos.y - zombieHeight,
                zombieWidth * 2,
                zombieHeight * 2
            );

            // 检查碰撞
            if (lawnMowerRect.intersectsRect(zombieRect)) {
                // 激活推车
                activateLawnMower(row);
                break; // 同一行只需要激活一次推车
            }
        }
    }
}
// 移除指定行的推车
void GameScene::removeLawnMower(int row) {
    if (row < 0 || row >= GRID_ROWS) {
        return;
    }

    if (lawnMowers[row]) {
        // 从场景中移除推车
        lawnMowers[row]->removeFromParent();
        lawnMowers[row] = nullptr;
        lawnMowerActive[row] = false;
    }
}
// 更新推车系统
void GameScene::updateLawnMowers(float dt) {
    // 检查推车和僵尸的碰撞
    checkLawnMowerCollision();
    // 更新激活的推车状态
    for (int row = 0; row < GRID_ROWS; row++) {
        if (lawnMowerActive[row] && lawnMowers[row]) {
            // 检查推车是否已经移出屏幕
            auto lawnMower = lawnMowers[row];
            if (lawnMower->getPositionX() > bgScreenEndX + 50) {
                removeLawnMower(row);
            }
        }
    }
}
// 重新生成推车（可以在需要时调用，比如新的一波僵尸到来时）
void GameScene::regenerateLawnMower(int row) {
    // 如果该行已经有推车，则不重新生成
    if (lawnMowers[row] != nullptr) {
        return;
    }

    // 延迟一段时间后重新生成推车
    this->scheduleOnce([this, row](float dt) {
        createLawnMowerForRow(row);
        }, 10.0f, "regenerate_lawnmower_" + std::to_string(row));
}

// 重置所有推车（游戏重新开始时调用）
void GameScene::resetLawnMowers() {
    for (int row = 0; row < GRID_ROWS; row++) {
        if (lawnMowers[row]) {
            lawnMowers[row]->removeFromParent();
            lawnMowers[row] = nullptr;
            lawnMowerActive[row] = false;
        }
        createLawnMowerForRow(row);
    }
}