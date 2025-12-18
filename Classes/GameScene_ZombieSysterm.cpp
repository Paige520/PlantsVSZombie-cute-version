#include "GameScene.h"
#include "Zombie.h"
#include <random>

USING_NS_CC;

// 初始化僵尸系统
void GameScene::initZombieSystem() {
    zombieSpawnTimer = 0.0f;
    zombieSpawnInterval = 15.0f / speed; // 初始间隔15秒，受速度影响

    // 初始化空僵尸数组
    zombies.clear();
}
// 随机生成僵尸
void GameScene::generateRandomZombie() {
    // 生成随机数决定僵尸类型
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    float randomValue = dis(gen);
    ZombieType zombieType;

    // 根据优先级决定僵尸类型
    if (randomValue < 0.5f) { // 50% 普通僵尸
        zombieType = ZombieType::NORMAL;
    }
    else if (randomValue < 0.8f) { // 30% 路障僵尸（累计80%）
        zombieType = ZombieType::CONE;
    }
    else if (randomValue < 0.95f) { // 15% 铁桶僵尸（累计95%）
        zombieType = ZombieType::BUCKET;
    }
    else { // 5% 旗帜僵尸（累计100%）
        zombieType = ZombieType::FLAG;
    }

    // 随机选择行（0-4）
    std::uniform_int_distribution<> rowDis(0, GRID_ROWS - 1);
    int row = rowDis(gen);

    // 创建僵尸
    auto zombie = Zombie::createZombie(zombieType);
    if (!zombie) return;

    // 设置僵尸位置（从屏幕右侧外生成）
    float startX = bgScreenEndX + 100;
    Vec2 gridCenter = getGridCenter(row, 0);
    float yPos = gridCenter.y;

    zombie->setPosition(Vec2(startX, yPos));

    // 设置僵尸大小（适应网格）
    float zombieScale = cellHeight / zombie->getContentSize().height * 1.0f;
    zombie->setScale(zombieScale);

    // 添加僵尸到场景
    this->addChild(zombie, 5); // 在植物下方显示

    // 保存僵尸引用
    zombies.pushBack(zombie);

    // 如果是旗帜僵尸，增加移动速度
    if (zombieType == ZombieType::FLAG) {
        // 旗帜僵尸移动速度略快
        zombie->setSpeed(zombie->getSpeed() * 1.2f);
    }

    // 添加生成动画
    zombie->setOpacity(0);
    auto fadeIn = FadeIn::create(0.5f);
    zombie->runAction(fadeIn);
}
// 更新僵尸状态（每帧调用）
void GameScene::updateZombies(float dt) {
    // 更新僵尸生成计时器
    zombieSpawnTimer += dt;

    // 检查是否需要生成僵尸
    if (zombieSpawnTimer >= zombieSpawnInterval) {
        generateRandomZombie();
        zombieSpawnTimer = 0.0f;

        // 逐渐减少生成间隔，增加难度（但不会小于5秒）
        zombieSpawnInterval = std::max(5.0f / speed, zombieSpawnInterval - 0.5f);
    }

    // 更新所有僵尸状态
    for (auto& zombie : zombies) {
        if (zombie) {
            zombie->update(dt * speed); // 乘以速度因子

            // 检查僵尸是否到达房子（屏幕左侧）
            if (zombie->getPositionX() < bgScreenStartX - 50) {
                // 僵尸到达房子，游戏结束
                gameOver();
                break;
            }
        }
    }

    // 检查植物和僵尸的碰撞
    checkPlantZombieCollision();

    // 移除死亡的僵尸
    removeDeadZombies();
}
// 检查植物和僵尸的碰撞
void GameScene::checkPlantZombieCollision() {
    // 遍历所有网格检查是否有植物
    for (int row = 0; row < GRID_ROWS; row++) {
        for (int col = 0; col < GRID_COLS; col++) {
            auto plant = gridPlants[row][col];
            if (!plant) continue;

            // 获取植物位置和大小
            Vec2 plantPos = plant->getPosition();
            float plantWidth = plant->getContentSize().width * plant->getScale() * 0.5f;
            float plantHeight = plant->getContentSize().height * plant->getScale() * 0.5f;

            Rect plantRect = Rect(
                plantPos.x - plantWidth,
                plantPos.y - plantHeight,
                plantWidth * 2,
                plantHeight * 2
            );

            // 检查每个僵尸是否与植物碰撞
            for (auto& zombie : zombies) {
                if (!zombie || zombie->getHP() <= 0) continue;

                // 获取僵尸位置和大小
                Vec2 zombiePos = zombie->getPosition();
                float zombieWidth = zombie->getContentSize().width * zombie->getScale() * 0.3f;
                float zombieHeight = zombie->getContentSize().height * zombie->getScale() * 0.3f;

                Rect zombieRect = Rect(
                    zombiePos.x - zombieWidth,
                    zombiePos.y - zombieHeight,
                    zombieWidth * 2,
                    zombieHeight * 2
                );

                // 检查碰撞
                if (plantRect.intersectsRect(zombieRect)) {
                    // 僵尸攻击植物
                    // 这里可以添加植物的生命值减少逻辑
                    // 暂时先让植物消失（后续可以添加植物生命值）
                    plant->removeFromParent();
                    gridPlants[row][col] = nullptr;

                    // 一个僵尸一次只攻击一个植物
                    break;
                }
            }
        }
    }
}
// 移除死亡的僵尸
void GameScene::removeDeadZombies() {
    Vector<Zombie*> zombiesToRemove;

    for (auto& zombie : zombies) {
        if (zombie && zombie->getHP() <= 0) {
            zombiesToRemove.pushBack(zombie);

            // 添加死亡动画
            auto fadeOut = FadeOut::create(0.5f);
            auto remove = RemoveSelf::create();
            auto sequence = Sequence::create(fadeOut, remove, nullptr);
            zombie->runAction(sequence);
        }
    }

    // 从数组中移除
    for (auto& zombie : zombiesToRemove) {
        zombies.eraseObject(zombie);
    }
}
// 游戏结束函数
void GameScene::gameOver() {
    // 暂停游戏
    this->pause(); // 暂停场景中的动作
    this->unscheduleAllCallbacks(); // 停止所有调度器

    // 显示游戏结束画面
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建半透明背景
    auto gameOverLayer = LayerColor::create(Color4B(0, 0, 0, 180), visibleSize.width, visibleSize.height);
    gameOverLayer->setPosition(Vec2::ZERO);
    gameOverLayer->setTag(100); // 添加tag以便后续查找
    this->addChild(gameOverLayer, 100);

    // 显示僵尸胜利标志
    auto zombieWinFlag = Sprite::create("ZombiesWon.png");

    // 计算缩放比例，使图标占页面一半
    float desiredWidth = visibleSize.width * 0.5f; // 宽度占屏幕一半
    float originalWidth = zombieWinFlag->getContentSize().width;
    float scale = desiredWidth / originalWidth;

    zombieWinFlag->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 100));
    zombieWinFlag->setScale(scale);
    zombieWinFlag->setOpacity(255); // 完全不透明
    gameOverLayer->addChild(zombieWinFlag, 101);

    // 直接显示菜单（不需要等待）
    showGameOverMenu(visibleSize, gameOverLayer);
}
// 显示游戏结束菜单
void GameScene::showGameOverMenu(const cocos2d::Size& visibleSize, cocos2d::LayerColor* gameOverLayer) {
    // 按钮放大倍数
    float buttonScale = 2.0f; // 放大2倍

    auto restartButton = MenuItemImage::create(
        "OptionsButton.png",
        "OptionsButton.png",
        [this](Ref* sender) {
            auto button = dynamic_cast<MenuItemImage*>(sender);
            if (button) {
                // 添加按钮点击反馈
                auto scaleDown = ScaleTo::create(0.1f, button->getScale() * 0.9f);
                auto scaleUp = ScaleTo::create(0.1f, button->getScale());
                auto sequence = Sequence::create(scaleDown, scaleUp, nullptr);
                button->runAction(sequence);

                // 延迟执行重新开始
                this->runAction(Sequence::create(
                    DelayTime::create(0.1f),
                    CallFunc::create([this]() {
                        Director::getInstance()->resume();
                        this->restartGame();
                        }),
                    nullptr
                ));
            }
            else {
                Director::getInstance()->resume();
                this->restartGame();
            }
        }
    );

    if (restartButton) {
        // 设置按钮放大
        restartButton->setScale(buttonScale);

        auto restartLabel = Label::createWithSystemFont(u8"重新开始", "Arial", 36); // 增大字体
        restartLabel->setPosition(Vec2(
            restartButton->getContentSize().width / 2,
            restartButton->getContentSize().height / 2
        ));
        restartLabel->setColor(Color3B::WHITE);
        restartLabel->enableOutline(Color4B::BLACK, 3); // 增大描边
        restartButton->addChild(restartLabel, 101);
    }

    // 重新开始按钮放在左下方
    restartButton->setPosition(Vec2(-visibleSize.width * 0.25, -visibleSize.height * 0.4 + 50));

    // 创建退出按钮
    auto exitButton = MenuItemImage::create(
        "OptionsButton.png",
        "OptionsButton.png",
        [this](Ref* sender) {
            auto button = dynamic_cast<MenuItemImage*>(sender);
            if (button) {
                // 添加按钮点击反馈
                auto scaleDown = ScaleTo::create(0.1f, button->getScale() * 0.9f);
                auto scaleUp = ScaleTo::create(0.1f, button->getScale());
                auto sequence = Sequence::create(scaleDown, scaleUp, nullptr);
                button->runAction(sequence);

                // 延迟执行退出
                this->runAction(Sequence::create(
                    DelayTime::create(0.1f),
                    CallFunc::create([this]() {
                        Director::getInstance()->resume();
                        this->exitToMainMenu();
                        }),
                    nullptr
                ));
            }
            else {
                Director::getInstance()->resume();
                this->exitToMainMenu();
            }
        }
    );

    if (exitButton) {
        // 设置按钮放大
        exitButton->setScale(buttonScale);

        auto exitLabel = Label::createWithSystemFont(u8"退出游戏", "Arial", 36); // 增大字体
        exitLabel->setPosition(Vec2(
            exitButton->getContentSize().width / 2,
            exitButton->getContentSize().height / 2
        ));
        exitLabel->setColor(Color3B::WHITE);
        exitLabel->enableOutline(Color4B::BLACK, 3); // 增大描边
        exitButton->addChild(exitLabel, 101);
    }

    // 退出按钮放在右下方
    exitButton->setPosition(Vec2(visibleSize.width * 0.25, -visibleSize.height * 0.4 + 50));

    // 创建菜单
    auto menu = Menu::create(restartButton, exitButton, nullptr);
    menu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    menu->setTag(102); // 设置tag以便查找
    gameOverLayer->addChild(menu);
}// 重写update函数
void GameScene::update(float dt) {
    // 更新僵尸系统
    updateZombies(dt);
    updateLawnMowers(dt);
}
