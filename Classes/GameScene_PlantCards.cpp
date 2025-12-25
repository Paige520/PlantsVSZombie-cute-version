#include "GameScene.h"
#include "Plant.h"

USING_NS_CC;
//初始卡片
void GameScene::initPlantCards() {
    // 获取商店位置和大小
    float shopX = shop->getPositionX();
    float shopY = shop->getPositionY();
    float shopWidth = shop->getContentSize().width * shop->getScaleX();
    float shopHeight = shop->getContentSize().height * shop->getScaleY();

    // 计算卡片起始位置（在商店内部）
    float startX = shopX - shopWidth / 2 + 280;
    float startY = shopY - shopHeight / 2 + 120;
    float spacing = 150; // 卡片间距
    float cardScale = 1.5f;

    // 创建植物卡片
    std::vector<PlantType> plants = {
        PlantType::PEASHOOTER,
        PlantType::SNOWPEA,
        PlantType::SUNFLOWER,
        PlantType::WALLNUT,
        PlantType::FIRETREE,
        PlantType::CHERRYBOMB
    };

    for (int i = 0; i < plants.size(); i++) {
        Vec2 position(startX + i * spacing, startY);
        createPlantCard(plants[i], position, cardScale);
    }
}
//创建卡片
void GameScene::createPlantCard(PlantType type, const Vec2& position, float cardScale) {
    // 创建卡片背景
    auto card = Sprite::create("Card.png");
    if (!card) return;

    // 计算合适的卡片大小
    card->setScale(cardScale);
    card->setPosition(position);

    // 根据植物类型设置图标
    std::string plantImage;
    switch (type) {
    case PlantType::PEASHOOTER: plantImage = "pea-shooter.png"; break;
    case PlantType::SNOWPEA: plantImage = "ice-pea-shooter.png"; break;
    case PlantType::SUNFLOWER: plantImage = "sunflower.png"; break;
    case PlantType::WALLNUT: plantImage = "nut.png"; break;
    case PlantType::FIRETREE: plantImage = "firetree.png"; break;
    case PlantType::CHERRYBOMB: plantImage = "cherrybomb.png"; break;
    default: return;
    }

    // 添加植物图标
    auto plantIcon = Sprite::create(plantImage);
    if (plantIcon) {
        float iconScale = 0.12f;
        plantIcon->setScale(iconScale);
        plantIcon->setPosition(Vec2(card->getContentSize().width / 2,
            card->getContentSize().height * 0.55f));
        card->addChild(plantIcon);
    }

    // 添加阳光价格标签
    int price = Plant::getCost(type);
    auto priceLabel = Label::createWithSystemFont(std::to_string(price), "Arial", 22);
    if (priceLabel) {
        priceLabel->setPosition(Vec2(card->getContentSize().width / 2 - 10,
            card->getContentSize().height * 0.1f));
        priceLabel->setColor(Color3B::BLACK);
        card->addChild(priceLabel);
    }

    // 添加触摸事件
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);

    listener->onTouchBegan = [this, card, type, price](Touch* touch, Event* event) {
        Vec2 touchLocation = touch->getLocation();
        Rect cardRect = card->getBoundingBox();

        if (cardRect.containsPoint(touchLocation)) {
            // 检查卡片是否在冷却中
            if (cardCooldownTimers[type] > 0.0f) {
                // 冷却中，提示效果
                auto shake = Sequence::create(
                    MoveBy::create(0.05f, Vec2(5, 0)),
                    MoveBy::create(0.05f, Vec2(-10, 0)),
                    MoveBy::create(0.05f, Vec2(5, 0)),
                    nullptr
                );
                card->runAction(shake);
                return false;
            }

            // 检查阳光是否足够
            if (sunshineCount >= price) {
                this->selectPlantCard(card, type);
                return true;
            }
            else {
                // 阳光不足，提示效果
                auto shake = Sequence::create(
                    MoveBy::create(0.05f, Vec2(5, 0)),
                    MoveBy::create(0.05f, Vec2(-10, 0)),
                    MoveBy::create(0.05f, Vec2(5, 0)),
                    nullptr
                );
                card->runAction(shake);

                // 显示红色提示
                card->setColor(Color3B::RED);
                card->runAction(Sequence::create(
                    DelayTime::create(0.3f),
                    CallFunc::create([card]() {
                        card->setColor(Color3B::WHITE);
                        }),
                    nullptr
                ));
            }
        }
        return false;
        };

    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, card);

    // 创建冷却遮罩
    auto cooldownMask = Sprite::create("Card.png");
    if (cooldownMask) {
        cooldownMask->setScale(cardScale);
        cooldownMask->setPosition(position);
        cooldownMask->setColor(Color3B::GRAY);
        cooldownMask->setOpacity(150);
        cooldownMask->setVisible(false); // 默认隐藏
        this->addChild(cooldownMask, 11); // 遮罩在卡片上方
        cardCooldownMasks.pushBack(cooldownMask);
    }

    // 设置植物类型的冷却时间
    float cooldownTime = 0.0f;
    switch (type) {
    case PlantType::PEASHOOTER:
        cooldownTime = 7.5f; // 7.5秒冷却
        break;
    case PlantType::SNOWPEA:
        cooldownTime = 7.5f;
        break;
    case PlantType::SUNFLOWER:
        cooldownTime = 7.5f;
        break;
    case PlantType::WALLNUT:
        cooldownTime = 30.0f; // 坚果冷却时间较长
        break;
    case PlantType::FIRETREE:
        cooldownTime = 7.5f;
        break;
    case PlantType::CHERRYBOMB:
        cooldownTime = 30.0f; // 樱桃炸弹冷却时间较长
        break;
    default:
        cooldownTime = 0.0f;
        break;
    }
    cardCooldowns[type] = cooldownTime;
    cardCooldownTimers[type] = 0.0f;

    // 建立植物类型到卡片和冷却遮罩的映射
    plantTypeToCardMap[type] = card;
    if (cooldownMask) {
        plantTypeToCooldownMaskMap[type] = cooldownMask;
    }

    // 保存卡片引用
    plantCards.pushBack(card);
    this->addChild(card, 10);
}
//选择卡片
void GameScene::selectPlantCard(Sprite* card, PlantType type) {
    // 如果已经选中了同一个卡片，取消选择
    if (selectedCard == card) {
        unselectPlantCard();
        return;
    }

    // 取消之前的选择
    unselectPlantCard();

    // 设置新选择
    selectedCard = card;
    selectedPlant = type;

    // 高亮显示选中的卡片
    auto scaleUp = ScaleTo::create(0.1f, card->getScale() * 1.2f);
    auto glow = TintTo::create(0.1f, 200, 255, 200);
    card->runAction(scaleUp);
    card->runAction(glow);

    // 显示种植预览
    showPlantingPreview();
}
//取消选择卡片
void GameScene::unselectPlantCard() {
    if (selectedCard) {
        // 恢复原始状态
        auto scaleDown = ScaleTo::create(0.1f, 1.5f);
        auto unglow = TintTo::create(0.1f, 255, 255, 255);
        selectedCard->runAction(scaleDown);
        selectedCard->runAction(unglow);

        selectedCard = nullptr;
    }

    selectedPlant = PlantType::NONE;
    hidePlantingPreview();
}
//种植预览
void GameScene::showPlantingPreview() {
    if (selectedPlant == PlantType::NONE || !selectedCard) return;

    std::string plantImage;
    switch (selectedPlant) {
    case PlantType::PEASHOOTER: plantImage = "pea-shooter.png"; break;
    case PlantType::SNOWPEA: plantImage = "ice-pea-shooter.png"; break;
    case PlantType::SUNFLOWER: plantImage = "sunflower.png"; break;
    case PlantType::WALLNUT: plantImage = "nut.png"; break;
    case PlantType::FIRETREE: plantImage = "firetree.png"; break;
    case PlantType::CHERRYBOMB: plantImage = "cherrybomb.png"; break;
    default: return;
    }

    plantingPreview = Sprite::create(plantImage);
    if (!plantingPreview) return;

    // 设置预览效果
    float previewScale = cellWidth / plantingPreview->getContentSize().width * 0.8f;
    plantingPreview->setScale(previewScale);
    plantingPreview->setOpacity(200);
    plantingPreview->setVisible(false);

    this->addChild(plantingPreview, 15);
}
//隐藏预览
void GameScene::hidePlantingPreview() {
    if (plantingPreview) {
        plantingPreview->removeFromParent();
        plantingPreview = nullptr;
    }
}