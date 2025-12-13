#include "SelectScene.h"
#include "GameScene.h"
#include "audio/include/AudioEngine.h"
#include<string>
using namespace cocos2d::experimental;

USING_NS_CC;

Scene* SelectScene::createScene() {
	return SelectScene::create();
}

bool SelectScene::init() {
	if (!Scene::init())
		return false;

	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();

	//初始化背景
    auto background = Sprite::create("selectbackground.jpg"); 
	background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	auto bgSize = background->getContentSize();
	float scaleX = visibleSize.width / bgSize.width;
	float scaleY = visibleSize.height / bgSize.height;
	float scale = MAX(scaleX, scaleY);
	background->setScale(scale);
	this->addChild(background, 0);

	createStartButton();
	createExitButton();

	return true;
}

void SelectScene::createStartButton() {
	auto visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();
	//添加关卡按钮
	auto button = Sprite::create("choice.png");
	button->setPosition(Vec2(visibleSize.width *0.75, visibleSize.height * 0.6f));
	button->setScale(1.0f);
	this->addChild(button, 2);

	// 为按钮添加触摸监听器
	auto listener = EventListenerTouchOneByOne::create();
	listener->setSwallowTouches(true);

    // 存储按钮的引用，在lambda中使用
    listener->onTouchBegan = [this, button](Touch* touch, Event* event) {
        // 检查是否点击在按钮上 - 使用更简单的检测方法
        Vec2 touchLocation = touch->getLocation();
        Vec2 buttonPosition = button->getPosition();

        // 获取按钮的实际大小（考虑缩放）
        auto contentSize = button->getContentSize();
        float scaledWidth = contentSize.width * button->getScaleX();
        float scaledHeight = contentSize.height * button->getScaleY();

        // 创建按钮的矩形区域
        float left = buttonPosition.x - scaledWidth / 2;
        float right = buttonPosition.x + scaledWidth / 2;
        float bottom = buttonPosition.y - scaledHeight / 2;
        float top = buttonPosition.y + scaledHeight / 2;

        // 检查触摸点是否在按钮区域内
        if (touchLocation.x >= left && touchLocation.x <= right &&
            touchLocation.y >= bottom && touchLocation.y <= top) {

            // 添加点击动画效果
            auto scaleUp = ScaleTo::create(0.1f, 1.1f);
            auto scaleDown = ScaleTo::create(0.1f, 1.0f);
            auto sequence = Sequence::create(scaleUp, scaleDown, nullptr);
            button->runAction(sequence);

            // 延迟一小段时间后跳转到游戏场景
            this->runAction(Sequence::create(
                DelayTime::create(0.2f),
                CallFunc::create([this]() {
                    // 切换到游戏场景
                    auto gameScene = GameScene::createScene();
                    auto transition = TransitionFade::create(1.0f, gameScene, Color3B::BLACK);
                    Director::getInstance()->replaceScene(transition);
                    }),
                nullptr
            ));
            return true; // 触摸被处理
        }
        return false; // 触摸未被处理
        };

    // 将监听器添加到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, button);
}
void SelectScene::createExitButton() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    Vec2 origin = Director::getInstance()->getVisibleOrigin();

    // 创建退出文字按钮
    auto exitLabel = Label::createWithSystemFont(u8"退出游戏", "Arial", 36);
    exitLabel->setPosition(Vec2(visibleSize.width *0.8, visibleSize.height * 0.1f));
    exitLabel->setColor(Color3B::BLACK);
    this->addChild(exitLabel, 2);

    // 为退出按钮添加触摸监听器
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this,exitLabel](Touch* touch, Event* event) {
        // 检查是否点击在退出按钮上
        Vec2 touchLocation = touch->getLocation();
        Vec2 labelPosition = exitLabel->getPosition();

        // 获取标签的实际大小
        auto contentSize = exitLabel->getContentSize();

        // 创建标签的矩形区域
        float left = labelPosition.x - contentSize.width / 2;
        float right = labelPosition.x + contentSize.width / 2;
        float bottom = labelPosition.y - contentSize.height / 2;
        float top = labelPosition.y + contentSize.height / 2;
        if (touchLocation.x >= left && touchLocation.x <= right &&
            touchLocation.y >= bottom && touchLocation.y <= top) {
            // 添加点击动画效果
            exitLabel->setColor(Color3B::YELLOW);
            auto scaleUp = ScaleTo::create(0.1f, 1.1f);
            auto scaleDown = ScaleTo::create(0.1f, 1.0f);
            auto sequence = Sequence::create(scaleUp, scaleDown, nullptr);
            exitLabel->runAction(sequence);

            // 延迟一小段时间后退出游戏
            auto scene = Director::getInstance()->getRunningScene();
            scene->runAction(Sequence::create(
                DelayTime::create(0.2f),
                CallFunc::create([]() {
                    // 退出游戏
                    Director::getInstance()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
                    exit(0);
#endif
                    }),
                nullptr
            ));
            return true;
        }
        return false;
        };

    // 将监听器添加到事件分发器
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, exitLabel);
}