#include "StartScene.h"
#include "GameScene.h"
#include <iostream>

USING_NS_CC;

Scene* StartScene::createScene() {
    return StartScene::create();
}

bool StartScene::init() {
    if (!Scene::init()) {
        return false;
    }
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // ==================== 设置图片背景 ====================
    Sprite* background = Sprite::create("startscene.jpg");
    // 设置图片背景的位置和缩放
    auto bgSize = background->getContentSize();

    // 将背景图放置在屏幕中央
    background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    // 计算缩放比例以适应屏幕
    float scaleX = visibleSize.width / bgSize.width;
    float scaleY = visibleSize.height / bgSize.height;

    float scale = MAX(scaleX, scaleY);
    background->setScale(scale);

    this->addChild(background, 0);

    //提示
    std::string tapHintStr = u8"点击屏幕进入游戏"; // u8前缀确保UTF-8编码
    Label* tapHint = Label::createWithSystemFont(tapHintStr, "Arial", 42);
    tapHint->enableShadow();
    tapHint->setColor(Color3B::BLACK); 
    tapHint->enableOutline(Color4B::WHITE, 2); 
    tapHint->setPosition(Vec2(visibleSize.width / 2, visibleSize.height * 0.4f));
    tapHint->setTag(200); // 设置标签，用于动画
    this->addChild(tapHint, 2);

    // 添加闪烁动画
    auto fadeOut = FadeOut::create(1.0f);
    auto fadeIn = FadeIn::create(1.0f);
    auto blinkSequence = Sequence::create(fadeOut, fadeIn, nullptr);
    tapHint->runAction(RepeatForever::create(blinkSequence));

    //进入游戏
    auto touchListener = EventListenerTouchOneByOne::create();
    touchListener->onTouchBegan = [this](Touch* touch, Event* event) {
        // 点击屏幕任意位置进入游戏
        this->transitionToGameScene();
        return true;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);
    // 也可以按任意键进入游戏
    auto keyboardListener = EventListenerKeyboard::create();
    keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        this->transitionToGameScene();
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    return true;
}
void StartScene::transitionToGameScene() {
    // 停止所有动画
    this->stopAllActions();

    // 获取并停止提示文字的闪烁动画
    auto tapHint = dynamic_cast<Label*>(this->getChildByTag(200));
    if (tapHint) {
        tapHint->stopAllActions();
        tapHint->setString(u8"进入游戏中...");
        tapHint->setColor(Color3B::GREEN);
    }

    // 创建并播放点击反馈效果
    auto touchEffect = ParticleSystemQuad::create();
    if (touchEffect) {
        touchEffect->setTotalParticles(30);
        touchEffect->setStartColor(Color4F::YELLOW);
        touchEffect->setEndColor(Color4F::RED);
        touchEffect->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2,
            Director::getInstance()->getVisibleSize().height / 2));
        this->addChild(touchEffect, 10);

        touchEffect->runAction(Sequence::create(
            DelayTime::create(0.5f),
            FadeOut::create(0.5f),
            RemoveSelf::create(),
            nullptr
        ));
    }

    // 延迟0.5秒后切换场景，让用户看到反馈效果
    this->runAction(Sequence::create(
        DelayTime::create(0.5f),
        CallFunc::create([this]() {
            auto scene = GameScene::createScene();
            auto transition = TransitionFade::create(1.0f, scene, Color3B::BLACK);
            Director::getInstance()->replaceScene(transition);
            }),
        nullptr
    ));
}