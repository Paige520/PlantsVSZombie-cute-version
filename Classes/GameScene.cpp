#include "GameScene.h"
#include "SelectScene.h"
#include "Plant.h"
#include "Zombie.h"
#include <iostream>
#include <random>

USING_NS_CC;

Scene* GameScene::createScene() {
    return GameScene::create();
}
//初始化
bool GameScene::init() {
    if (!Scene::init()) {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 初始化变量
    sunshineCount = 50;
    scaledBgWidth = 0;
    scaledBgHeight = 0;
    speed = 1.0;
    selectedPlant = PlantType::NONE; // 初始没有选中植物
    selectedCard = nullptr;
    plantingPreview = nullptr;

    for (int i = 0; i < GRID_ROWS; i++) {
        for (int j = 0; j < GRID_COLS; j++) {
            gridPlants[i][j] = nullptr;
        }
    }

    initBackground();      // 创建背景
    initShop();            // 创建商店
    initGrid();            // 创建网格
    initSunshineSystem();  // 创建阳光系统
    initPlantCards();      // 初始化植物卡片
    createPauseButton();   // 创建暂停按钮
    createPauseMenu();     // 创建暂停菜单
    initZombieSystem();    // 初始化僵尸系统
    initLawnMowerSystem(); // 初始化推车系统
    initAudioSystem();     // 初始化音频系统
    initBulletSystem();    // 初始化子弹系统

    // 添加触摸监听器用于种植
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (selectedPlant != PlantType::NONE) {
            // 如果选中了植物，检查是否可以种植
            auto gridPos = this->getGridFromPosition(touch->getLocation());
            int row = gridPos.first;
            int col = gridPos.second;

            if (row != -1 && col != -1) {
                if (this->canPlacePlant(row, col)) {
                    this->placePlant(row, col);
                    return true;
                }
            }
            // 点击非网格区域取消选择
            this->unselectPlantCard();
        }
        return false;
        };

    touchListener->onTouchMoved = [this](Touch* touch, Event* event) {
        if (selectedPlant != PlantType::NONE && plantingPreview) {
            // 移动预览
            auto gridPos = this->getGridFromPosition(touch->getLocation());
            int row = gridPos.first;
            int col = gridPos.second;

            if (row != -1 && col != -1) {
                if (this->canPlacePlant(row, col)) {
                    plantingPreview->setColor(Color3B::WHITE);
                    plantingPreview->setOpacity(200);
                }
                else {
                    plantingPreview->setColor(Color3B::RED);
                    plantingPreview->setOpacity(150);
                }
                plantingPreview->setPosition(this->getGridCenter(row, col));
            }
            else {
                plantingPreview->setVisible(false);
            }
        }
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // 开始定时生成阳光
    this->schedule([this](float dt) {
        this->generateSunshine();
        }, 5.0f, "generate_sunshine");  // 每5秒生成一个阳光

    // 开始更新僵尸状态（每帧调用）
    this->scheduleUpdate();
    return true;
}

// 初始化子弹系统
void GameScene::initBulletSystem() {
    // 清空子弹容器
    bullets.clear();
}

// 获取阳光容器的引用
Vector<Sprite*>& GameScene::getSunshines() {
    return sunshines;
}

// 获取僵尸容器的引用
Vector<Zombie*>& GameScene::getZombies() {
    return zombies;
}

// 更新方法，处理所有游戏逻辑更新
void GameScene::update(float dt) {
    // 更新植物卡片冷却时间
    for (auto& pair : cardCooldownTimers) {
        PlantType plantType = pair.first;
        float& timer = pair.second;

        if (timer > 0.0f) {
            timer -= dt;

            // 更新冷却遮罩的显示
            auto maskIter = plantTypeToCooldownMaskMap.find(plantType);
            if (maskIter != plantTypeToCooldownMaskMap.end()) {
                auto mask = maskIter->second;
                float cooldownTime = cardCooldowns[plantType];
                if (cooldownTime > 0.0f) {
                    // 计算当前冷却进度（0.0 - 1.0）
                    float progress = timer / cooldownTime;

                    // 更新遮罩的显示
                    if (progress > 0.0f) {
                        mask->setVisible(true);
                        // 这里可以添加进度条效果，比如遮罩的透明度或高度变化
                    }
                    else {
                        mask->setVisible(false);
                        timer = 0.0f;
                    }
                }
            }
        }
    }

    // 更新推车系统
    updateLawnMowers(dt);

    // 更新僵尸系统
    updateZombies(dt);

    // 更新子弹系统
    updateBullets(dt);   
}
