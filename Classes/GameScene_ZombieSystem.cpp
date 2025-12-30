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

    // 设置场景引用，让僵尸可以获取游戏速度
    zombie->setScene(this);

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
            zombie->update(dt);  //游戏速度已经通过setScene和updateSpeedWithGameSpeed应用到僵尸的m_speed中了
            zombie->updateSpeedWithGameSpeed(speed);
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
                    // 将Sprite指针转换为Plant指针
                    Plant* plantObj = dynamic_cast<Plant*>(plant);
                    if (plantObj) {
                        // 设置僵尸的目标植物
                        zombie->setTarget(plantObj);

                        // 让僵尸进入攻击状态，触发啃咬动画
                        zombie->setState(ZombieState::ATTACKING);
                    }

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
        if (zombie && (zombie->getHP() <= 0 || zombie->getState() == ZombieState::DEAD)) {
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

// 处理樱桃炸弹爆炸伤害
void GameScene::handleCherryBombExplosion(int row, int col) {
    // 爆炸范围：3x3网格
    const int explosionRange = 1;
    // 爆炸伤害：确保能消灭铁桶僵尸（铁桶僵尸生命值是1300）
    const int explosionDamage = 2000;

    // 创建爆炸视觉特效
    Vec2 explosionPos = getGridCenter(row, col);

    // 简单的爆炸动画：缩放和颜色变化
    auto explosionSprite = Sprite::create("cherrybomb.png");
    if (explosionSprite) {
        explosionSprite->setPosition(explosionPos);
        explosionSprite->setScale(0.1f);
        this->addChild(explosionSprite, 50);

        // 爆炸动画
        auto scaleUp = ScaleTo::create(0.3f, 3.0f);
        auto fadeOut = FadeOut::create(0.3f);
        auto remove = RemoveSelf::create();
        auto sequence = Sequence::create(Spawn::create(scaleUp, fadeOut, nullptr), remove, nullptr);
        explosionSprite->runAction(sequence);

        // 添加红色闪光效果
        auto flash = Sprite::create("white_pixel.png");
        if (flash) {
            flash->setPosition(explosionPos);
            flash->setScale(0.1f);
            flash->setColor(Color3B::RED);
            flash->setOpacity(150);
            this->addChild(flash, 50);

            auto flashScale = ScaleTo::create(0.2f, 4.0f);
            auto flashFade = FadeOut::create(0.2f);
            auto flashRemove = RemoveSelf::create();
            auto flashSequence = Sequence::create(Spawn::create(flashScale, flashFade, nullptr), flashRemove, nullptr);
            flash->runAction(flashSequence);
        }
    }

    // 检查爆炸范围内的所有僵尸
    for (int r = row - explosionRange; r <= row + explosionRange; r++) {
        for (int c = col - explosionRange; c <= col + explosionRange; c++) {
            // 检查网格是否有效
            if (r < 0 || r >= GRID_ROWS || c < 0 || c >= GRID_COLS) {
                continue;
            }

            // 计算当前网格的中心位置
            Vec2 gridCenter = getGridCenter(r, c);

            // 检查每个僵尸是否在爆炸范围内
            for (auto& zombie : zombies) {
                if (!zombie || zombie->getHP() <= 0) {
                    continue;
                }

                Vec2 zombiePos = zombie->getPosition();
                // 计算僵尸与爆炸中心的距离
                float distance = zombiePos.getDistance(gridCenter);

                // 如果僵尸在爆炸范围内，造成伤害
                if (distance <= gridWidth) { // gridWidth作为爆炸半径
                    zombie->takeDamage(explosionDamage);

                    // 添加伤害效果
                    auto tintRed = TintTo::create(0.1f, 255, 0, 0);
                    auto tintBack = TintTo::create(0.1f, 255, 255, 255);
                    auto sequence = Sequence::create(tintRed, tintBack, nullptr);
                    zombie->runAction(sequence);
                }
            }
        }
    }
}

// 游戏结束函数
void GameScene::gameOver() {
    // 暂停游戏
    this->pause(); // 暂停场景中的动作
    this->unscheduleAllCallbacks(); // 停止所有调度器
    // 暂停背景音乐
    pauseBackgroundMusic();
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
}

// 更新子弹系统
void GameScene::updateBullets(float dt) {
    // 更新所有子弹状态
    for (auto& bullet : bullets) {
        if (bullet) {
            // 使用子弹自身的速度属性，避免使用未定义的全局speed变量
            bullet->update(dt);
        }
    }

    // 检查子弹与火炬树桩的碰撞（火焰效果转换）
    checkBulletFireTreeCollision();

    // 检查子弹与僵尸的碰撞
    checkBulletZombieCollision();

    // 移除死亡的子弹
    removeDeadBullets();
}

// 检查子弹与僵尸的碰撞
void GameScene::checkBulletZombieCollision() {
    // 遍历所有子弹
    for (auto& bullet : bullets) {
        if (!bullet) continue;

        // 获取子弹位置和大小
        Vec2 bulletPos = bullet->getPosition();
        float bulletWidth = bullet->getContentSize().width * bullet->getScale() * 0.5f;
        float bulletHeight = bullet->getContentSize().height * bullet->getScale() * 0.5f;

        Rect bulletRect = Rect(
            bulletPos.x - bulletWidth,
            bulletPos.y - bulletHeight,
            bulletWidth * 2,
            bulletHeight * 2
        );

        // 遍历所有僵尸
        for (auto& zombie : zombies) {
            if (!zombie || zombie->getHP() <= 0) continue;

            // 获取僵尸位置和大小
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
            if (bulletRect.intersectsRect(zombieRect)) {
                // 子弹击中僵尸，根据子弹类型处理
                switch (bullet->getBulletType()) {
                case BulletType::NORMAL:
                    // 普通子弹，直接减少生命值
                    zombie->takeDamage(bullet->getDamage());
                    break;
                case BulletType::ICE:
                    // 冰冻子弹，减少生命值并添加更显著的减速效果
                    zombie->takeDamage(bullet->getDamage());
                    zombie->applySlow(0.3f, 5.0f); // 70%减速，持续5秒
                    break;
                case BulletType::FIRE:
                    // 火焰子弹，造成额外伤害
                    zombie->takeDamage(bullet->getDamage() * 2); // 假设火焰子弹伤害翻倍
                    break;
                default:
                    break;
                }

                // 添加击中效果 - 确保资源加载安全
                std::string hitEffectFile = "bullet_hit.png";
                auto hitEffect = Sprite::create(hitEffectFile);
                if (hitEffect) {
                    hitEffect->setPosition(zombiePos);
                    hitEffect->setScale(0.5f);
                    this->addChild(hitEffect, 10);

                    // 添加消失动画
                    auto fadeOut = FadeOut::create(0.3f);
                    auto remove = RemoveSelf::create();
                    auto sequence = Sequence::create(fadeOut, remove, nullptr);
                    hitEffect->runAction(sequence);
                }
                else {
                    // 如果资源加载失败，记录日志但不崩溃
                    CCLOG("Warning: Failed to load bullet hit effect texture: %s", hitEffectFile.c_str());
                }

                // 标记子弹为死亡，由removeDeadBullets函数统一移除
                bullet->setIsDead(true);
                break; // 子弹只能击中一个僵尸
            }
        }

        // 检查子弹是否飞出屏幕右侧 - 标记为死亡，由removeDeadBullets函数统一移除
        if (bulletPos.x > bgScreenEndX + 100 && !bullet->getIsDead()) {
            bullet->setIsDead(true);
        }
    }
}

// 添加子弹到容器
void GameScene::addBullet(Bullet* bullet) {
    if (bullet) {
        bullets.pushBack(bullet);
    }
}

// 检查子弹与火炬树桩的碰撞（火焰效果转换）
void GameScene::checkBulletFireTreeCollision() {
    // 遍历所有子弹
    for (int i = 0; i < this->bullets.size(); ++i) {
        Bullet* bullet = this->bullets.at(i);
        if (!bullet) continue;

        // 只处理非火焰子弹
        if (bullet->getBulletType() == BulletType::FIRE) continue;

        // 获取子弹位置和大小
        Vec2 bulletPos = bullet->getPosition();
        float bulletWidth = bullet->getContentSize().width * bullet->getScale() * 0.5f;
        float bulletHeight = bullet->getContentSize().height * bullet->getScale() * 0.5f;

        Rect bulletRect = Rect(
            bulletPos.x - bulletWidth,
            bulletPos.y - bulletHeight,
            bulletWidth * 2,
            bulletHeight * 2
        );

        // 遍历所有网格检查是否有火炬树桩
        for (int row = 0; row < GRID_ROWS; row++) {
            for (int col = 0; col < GRID_COLS; col++) {
                Plant* plant = this->gridPlants[row][col];
                if (!plant) continue;

                // 检查是否是火炬树桩
                if (plant->getType() != PlantType::FIRETREE) continue;

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

                // 检查碰撞
                if (bulletRect.intersectsRect(plantRect)) {
                    // 转换为火焰子弹
                    bullet->setBulletType(BulletType::FIRE);
                    bullet->switchToFireBullet();

                    // 一个子弹只能被转换一次
                    break;
                }
            }
        }
    }
}

// 移除死亡的子弹
void GameScene::removeDeadBullets() {
    Vector<Bullet*> bulletsToRemove;

    // 先找出所有需要移除的子弹
    for (auto& bullet : bullets) {
        if (bullet && bullet->getIsDead()) {
            bulletsToRemove.pushBack(bullet);
        }
    }

    // 对需要移除的子弹执行动画并从容器中移除
    for (auto& bullet : bulletsToRemove) {
        if (bullet && bullet->getParent()) { // 确保子弹仍在场景中
            // 先执行动画，动画完成后再从容器中移除
            auto fadeOut = FadeOut::create(0.2f);
            auto remove = RemoveSelf::create();
            auto sequence = Sequence::create(fadeOut, remove, nullptr);
            bullet->runAction(sequence);

            // 从容器中移除
            bullets.eraseObject(bullet);
        }
    }
}