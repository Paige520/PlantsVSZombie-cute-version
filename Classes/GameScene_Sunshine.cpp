#include "GameScene.h"
#include <random>

USING_NS_CC;

//初始化阳光系统
void GameScene::initSunshineSystem() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    //获取商店的起始位置
    float shopCenterX = shop->getPositionX();
    float shopCenterY = shop->getPositionY();
    float shopWidth = shop->getContentSize().width * shop->getScaleX();
    float shopHeight = shop->getContentSize().height * shop->getScaleY();

    float shopstartX = shopCenterX - shopWidth / 2;
    float shopstartY = shopCenterY - shopHeight / 2;

    // 创建阳光图标（在商店左侧）
    sunshineIcon = Sprite::create("sunshine.png"); // 阳光图片
    if (sunshineIcon) {
        // 设置阳光图标位置（商店左侧）
        float iconX = shopstartX + 100;
        float iconY = shopstartY + 150;

        float sunshineScale = cellWidth / sunshineIcon->getContentSize().width;
        sunshineIcon->setPosition(Vec2(iconX, iconY));
        sunshineIcon->setScale(sunshineScale);
        this->addChild(sunshineIcon, 10);

        // 创建阳光数量标签
        sunshineLabel = Label::createWithSystemFont(std::to_string(sunshineCount), "Arial", 32);
        sunshineLabel->setPosition(Vec2(iconX + 10, iconY - 100)); // 在图标下侧显示数字
        sunshineLabel->setColor(Color3B::BLACK);
        this->addChild(sunshineLabel, 10);
    }
}
// 生成阳光（在商店下沿随机位置）
void GameScene::generateSunshine() {
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // 确保商店已初始化
    if (!shop) return;

    // 获取商店位置和大小
    float shopX = shop->getPositionX();
    float shopY = shop->getPositionY();
    float shopWidth = shop->getContentSize().width * shop->getScaleX();
    float shopHeight = shop->getContentSize().height * shop->getScaleY();

    // 计算商店下沿的Y坐标
    float shopBottomY = shopY - shopHeight / 2;

    // 在商店宽度范围内随机生成X坐标
    float minX = shopX - shopWidth / 2;
    float maxX = shopX + shopWidth / 2;

    // 使用随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(minX, maxX);

    float randomX = dis(gen);

    // 创建阳光（从商店下沿开始）
    Vec2 sunshinePos = Vec2(randomX, shopBottomY);
    createSunshine(sunshinePos);
}
// 创建阳光精灵
void GameScene::createSunshine(const Vec2& position) {
    // 创建阳光
    auto sunshine = Sprite::create("sunshine.png");

    if (!sunshine)
        return;
    // 将阳光添加到容器中
    sunshines.pushBack(sunshine);

    float sunshineScale = cellWidth / sunshine->getContentSize().width;
    sunshine->setPosition(position);
    sunshine->setTag(100); // 设置标签，用于标识阳光

    // 添加阳光生成的平滑入场动画
    // 1. 初始状态：缩小、透明
    sunshine->setScale(0.1f);
    sunshine->setOpacity(0);
    
    // 2. 入场动画：放大到正常大小、淡入、旋转
    auto scaleTo = ScaleTo::create(0.5f, sunshineScale);
    auto fadeIn = FadeIn::create(0.5f);
    auto rotate = RotateTo::create(0.5f, 360);
    
    // 3. 播放入场动画
    auto spawn = Spawn::create(scaleTo, fadeIn, rotate, nullptr);
    sunshine->runAction(spawn);
    
    // 4. 入场后添加阳光闪烁效果
    auto fadeOut = FadeTo::create(0.5f, 200);
    auto fadeInLoop = FadeTo::create(0.5f, 255);
    auto blink = Sequence::create(fadeOut, fadeInLoop, nullptr);
    auto blinkForever = RepeatForever::create(blink);
    sunshine->runAction(Sequence::create(DelayTime::create(0.5f), blinkForever, nullptr));

    // 添加下落动画：优化阳光掉落效果，确保垂直下落
    // 获取阳光生成位置对应的格子坐标
    std::pair<int, int> gridPos = getGridFromPosition(position);
    int row = gridPos.first;
    int col = gridPos.second;
    
    // 计算该格子的中心位置作为下落目标（确保垂直下落）
    Vec2 targetPos = getGridCenter(row, col);
    // 确保阳光垂直下落，X坐标保持不变
    targetPos.x = position.x;
    
    // 优化下落动画，使其更加流畅自然
    auto moveDown = MoveTo::create(4.0f / speed, targetPos);
    auto easeInOut = EaseInOut::create(moveDown, 2.0f); // 使用EaseInOut使动画更自然

    sunshine->runAction(Sequence::create(
        easeInOut,
        CallFunc::create([sunshine]() {
            // 落到草坪后，轻微晃动
            auto shake = Sequence::create(
                RotateTo::create(0.1f, 10),
                RotateTo::create(0.1f, -10),
                RotateTo::create(0.1f, 0),
                DelayTime::create(0.5f),
                nullptr
            );
            sunshine->runAction(RepeatForever::create(shake));
            }),
        nullptr
    ));

    this->addChild(sunshine, 20);

    // 为阳光添加点击事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this, sunshine](Touch* touch, Event* event) {
        // 检查是否点击在阳光上
        Vec2 touchLocation = touch->getLocation();
        Vec2 localPos = sunshine->convertToNodeSpace(touchLocation);
        Rect rect = Rect(0, 0, sunshine->getContentSize().width, sunshine->getContentSize().height);

        if (rect.containsPoint(localPos)) {
            // 收集阳光
            this->collectSunshine(sunshine);
            return true;
        }
        return false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, sunshine);

    // 阳光如果在15秒内没有被收集，自动消失
    sunshine->runAction(Sequence::create(
        DelayTime::create(10.0f),
        CallFunc::create([this, sunshine]() {
            // 从容器中移除
            sunshines.eraseObject(sunshine);
            // 淡出效果
            auto fadeOut = FadeOut::create(0.5f);
            auto remove = RemoveSelf::create();
            sunshine->runAction(Sequence::create(fadeOut, remove, nullptr));
            }),
        nullptr
    ));
}
// 收集阳光
void GameScene::collectSunshine(Sprite* sunshine) {
    if (!sunshine)
        return;
    // 从容器中移除
    sunshines.eraseObject(sunshine);
    // 停止所有动画
    sunshine->stopAllActions();

    // 获取阳光图标位置
    if (!sunshineIcon) return;
    Vec2 targetPos = sunshineIcon->getPosition();

    // 创建收集动画
    auto moveTo = MoveTo::create(0.5f, targetPos);
    auto scaleDown = ScaleTo::create(0.5f, 0.1f);
    auto fadeOut = FadeOut::create(0.5f);

    auto spawn = Spawn::create(moveTo, scaleDown, fadeOut, nullptr);

    sunshine->runAction(Sequence::create(
        spawn,
        CallFunc::create([this, sunshine]() {
            // 更新阳光数量
            this->updateSunshineCount(50);
            // 移除阳光精灵
            sunshine->removeFromParent();
            }),
        nullptr
    ));
}
// 更新阳光数量
void GameScene::updateSunshineCount(int delta) {
    sunshineCount += delta;

    // 确保阳光数量不为负数
    if (sunshineCount < 0) {
        sunshineCount = 0;
    }

    // 更新显示
    if (sunshineLabel) {
        sunshineLabel->setString(std::to_string(sunshineCount));

        // 添加数值变化动画
        auto scaleUp = ScaleTo::create(0.1f, 1.3f);
        auto scaleDown = ScaleTo::create(0.1f, 1.0f);
        auto sequence = Sequence::create(scaleUp, scaleDown, nullptr);
        sunshineLabel->runAction(sequence);
    }
}