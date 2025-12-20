#include "GameScene.h"
#include "SelectScene.h"

USING_NS_CC;

// 创建暂停按钮
void GameScene::createPauseButton() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    // 创建暂停按钮
    auto pauseButton = MenuItemImage::create(
        "Button.png",
        "Button.png",
        [this](Ref* sender) {
            auto button = dynamic_cast<MenuItemImage*>(sender);
            if (button) {
                // 添加点击反馈效果
                auto scaleDown = ScaleTo::create(0.1f, 0.9f);
                auto scaleUp = ScaleTo::create(0.1f, 1.0f);
                auto sequence = Sequence::create(scaleDown, scaleUp, nullptr);
                button->runAction(sequence);

                // 延迟一小段时间后显示暂停菜单
                this->runAction(Sequence::create(
                    DelayTime::create(0.1f),
                    CallFunc::create([this]() {
                        this->showPauseMenu();
                        }),
                    nullptr
                ));
            }
            else {
                this->showPauseMenu();
            }
        }
    );

    pauseButton->setPosition(Vec2(
        bgScreenEndX - pauseButton->getContentSize().width * 0.7f,
        bgScreenEndY - pauseButton->getContentSize().height * 0.7f
    ));
    pauseButton->setScale(1.5f);
    // 在按钮上添加文字
    if (pauseButton) {
        auto pauseLabel = Label::createWithSystemFont(u8"暂停", "Arial", 28);
        pauseLabel->setPosition(Vec2(
            pauseButton->getContentSize().width / 2,
            pauseButton->getContentSize().height / 2
        ));
        pauseLabel->setColor(Color3B::BLACK);
        pauseButton->addChild(pauseLabel, 1);
    }
    auto menu = Menu::create(pauseButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    menu->setTag(999); // 设置标签，方便查找
    this->addChild(menu, 30);
}
// 创建暂停菜单界面
void GameScene::createPauseMenu() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 创建半透明背景层
    pauseLayer = LayerColor::create(Color4B(0, 0, 0, 180), visibleSize.width, visibleSize.height);
    pauseLayer->setPosition(Vec2::ZERO);
    pauseLayer->setVisible(false); // 初始隐藏
    this->addChild(pauseLayer, 40); // 设置更高的z-order

    // 创建暂停菜单背景框
    auto menuBg = Sprite::create("OptionsMenu.png");

    menuBg->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    menuBg->setScale(1.5f);
    pauseLayer->addChild(menuBg);

    auto resumeButton = MenuItemImage::create(
        "OptionsButton.png",  // 正常状态图片
        "OptionsButton.png",  // 按下状态图片
        [this](Ref* sender) {
            this->hidePauseMenu();
        }
    );

    // 在按钮上添加文字
    if (resumeButton) {
        auto resumeLabel = Label::createWithSystemFont(u8"继续游戏", "Arial", 28);
        resumeLabel->setPosition(Vec2(
            resumeButton->getContentSize().width / 2,
            resumeButton->getContentSize().height / 2
        ));
        resumeLabel->setColor(Color3B::WHITE);
        resumeLabel->enableOutline(Color4B::BLACK, 2);
        resumeButton->addChild(resumeLabel, 1);
    }
    resumeButton->setPosition(Vec2(0, 100));

    // 重新开始按钮
    auto restartButton = MenuItemImage::create(
        "OptionsButton.png",  // 正常状态图片
        "OptionsButton.png",  // 按下状态图片
        [this](Ref* sender) {
            this->restartGame();
        }
    );

    // 在按钮上添加文字
    if (restartButton) {
        auto restartLabel = Label::createWithSystemFont(u8"重新开始", "Arial", 28);
        restartLabel->setPosition(Vec2(
            restartButton->getContentSize().width / 2,
            restartButton->getContentSize().height / 2
        ));
        restartLabel->setColor(Color3B::WHITE);
        restartLabel->enableOutline(Color4B::BLACK, 2);
        restartButton->addChild(restartLabel, 1);
    }
    restartButton->setPosition(Vec2(0, 30));

    // 速度调节按钮
    auto speedButton = MenuItemImage::create(
        "OptionsButton.png",  // 正常状态图片
        "OptionsButton.png",  // 按下状态图片
        [this](Ref* sender) {
            this->changeGameSpeed();
        }
    );

    // 在按钮上添加文字
    if (speedButton) {
        auto speedButtonLabel = Label::createWithSystemFont(u8"游戏速度", "Arial", 28);
        speedButtonLabel->setPosition(Vec2(
            speedButton->getContentSize().width / 2,
            speedButton->getContentSize().height / 2
        ));
        speedButtonLabel->setColor(Color3B::WHITE);
        speedButtonLabel->enableOutline(Color4B::BLACK, 2);
        speedButton->addChild(speedButtonLabel, 1);
    }
    speedButton->setPosition(Vec2(0, -40));

    // 退出按钮
    auto exitButton = MenuItemImage::create(
        "OptionsButton.png",  // 正常状态图片
        "OptionsButton.png",  // 按下状态图片
        [this](Ref* sender) {
            this->exitToMainMenu();
        }
    );

    // 在按钮上添加文字
    if (exitButton) {
        auto exitLabel = Label::createWithSystemFont(u8"退出游戏", "Arial", 28);
        exitLabel->setPosition(Vec2(
            exitButton->getContentSize().width / 2,
            exitButton->getContentSize().height / 2
        ));
        exitLabel->setColor(Color3B::WHITE);
        exitLabel->enableOutline(Color4B::BLACK, 2);
        exitButton->addChild(exitLabel, 1);
    }
    exitButton->setPosition(Vec2(0, -110));

    // 速度显示标签
    speedLabel = Label::createWithSystemFont(u8"当前速度: 1.0x", "Arial", 24);
    speedLabel->setPosition(Vec2(
        visibleSize.width / 2,
        visibleSize.height / 2 - 170
    ));
    speedLabel->setColor(Color3B::WHITE);
    pauseLayer->addChild(speedLabel);

    // 创建菜单
    pauseMenu = Menu::create(resumeButton, restartButton, speedButton, exitButton, nullptr);
    pauseMenu->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    pauseLayer->addChild(pauseMenu);
}
// 显示暂停菜单
void GameScene::showPauseMenu() {
    // 暂停游戏
    Director::getInstance()->pause();

    // 暂停所有动作
    this->pause();
    //暂停音乐
    pauseBackgroundMusic();
    // 暂停所有植物卡片的触摸事件
    for (auto& card : plantCards) {
        card->pause();
    }

    // 暂停所有阳光的动画
    for (auto& sunshine : sunshines) {
        sunshine->pause();
    }
    // 暂停所有僵尸的动画
    for (auto& zombie : zombies) {
        zombie->pause();
    }

    // 暂停所有推车的动画
    for (int row = 0; row < GRID_ROWS; row++) {
        if (lawnMowers[row]) {
            lawnMowers[row]->pause();
        }
    }
    // 显示暂停菜单
    pauseLayer->setVisible(true);

    // 更新速度显示
    updateSpeedLabel();
}
// 隐藏暂停菜单
void GameScene::hidePauseMenu() {
    // 隐藏暂停菜单
    pauseLayer->setVisible(false);

    // 恢复所有植物卡片的触摸事件
    for (auto& card : plantCards) {
        card->resume();
    }
    // 恢复所有阳光的动画
    for (auto& sunshine : sunshines) {
        sunshine->resume();
    }

    // 恢复所有僵尸的动画
    for (auto& zombie : zombies) {
        zombie->resume();
    }

    // 恢复所有推车的动画
    for (int row = 0; row < GRID_ROWS; row++) {
        if (lawnMowers[row]) {
            lawnMowers[row]->resume();
        }
    }

    // 恢复背景音乐
    resumeBackgroundMusic();
  
    this->resume();
    Director::getInstance()->resume();
}
// 重新开始游戏
void GameScene::restartGame() {
    // 停止当前音乐
    stopBackgroundMusic();
    // 恢复游戏（确保在场景切换前恢复）
    Director::getInstance()->resume();
    // 重新加载当前场景
    auto scene = GameScene::createScene();
    auto transition = TransitionFade::create(0.5f, scene, Color3B::BLACK);
    Director::getInstance()->replaceScene(transition);
}
// 退出到主菜单
void GameScene::exitToMainMenu() {
    // 停止当前音乐
    stopBackgroundMusic();
    // 恢复游戏（确保在场景切换前恢复）
    Director::getInstance()->resume();

    auto scene = SelectScene::createScene();
    auto transition = TransitionFade::create(0.5f, scene, Color3B::BLACK);
    Director::getInstance()->replaceScene(transition);
}
// 切换游戏速度
void GameScene::changeGameSpeed() {
    // 切换速度：1.0 -> 1.5 -> 2.0 -> 1.0
    if (speed == 1.0f) {
        speed = 1.5f;
    }
    else if (speed == 1.5f) {
        speed = 2.0f;
    }
    else {
        speed = 1.0f;
    }

    // 更新速度显示
    updateSpeedLabel();

    // 重新调度阳光生成（根据新速度调整间隔）
    this->unschedule("generate_sunshine");
    float interval = 5.0f / speed; // 速度越快，生成间隔越短
    this->schedule([this](float dt) {
        this->generateSunshine();
        }, interval, "generate_sunshine");

}
// 更新速度显示
void GameScene::updateSpeedLabel() {
    if (speedLabel) {
        char speedText[50];
        sprintf(speedText, u8"当前速度: %.1fx", speed);
        speedLabel->setString(speedText);

        // 根据速度设置不同颜色
        if (speed == 1.0f) {
            speedLabel->setColor(Color3B::GREEN);
        }
        else if (speed == 1.5f) {
            speedLabel->setColor(Color3B::YELLOW);
        }
        else {
            speedLabel->setColor(Color3B::RED);
        }
    }
}