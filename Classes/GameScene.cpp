#include "GameScene.h"
#include "SelectScene.h"
#include "Plant.h"
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

    initBackground();
    initShop();
    initGrid();
    initSunshineSystem(); 
    initPlantCards();
    createPauseButton();
    createPauseMenu();

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
    return true;
}
//创建背景
void GameScene::initBackground() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
   
    // 计算背景图信息
    calculateBackgroundInfo();
    // 设置背景图
    background->setScale(scaledBgWidth / background->getContentSize().width);
    background->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(background, 0);
  
}
// 计算背景图信息
void GameScene::calculateBackgroundInfo() {
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto bgSize = background->getContentSize();

    // 计算缩放比例以适应屏幕
    float scaleX = visibleSize.width / bgSize.width;
    float scaleY = visibleSize.height / bgSize.height;

    // 选择最小缩放比例，确保背景图完整显示
    float scale = MIN(scaleX, scaleY);

    // 计算缩放后的尺寸
    scaledBgWidth = bgSize.width * scale;
    scaledBgHeight = bgSize.height * scale;

    // 计算背景图在屏幕上的显示区域（居中显示）
    bgScreenStartX = visibleSize.width / 2 - scaledBgWidth / 2;
    bgScreenStartY = visibleSize.height / 2 - scaledBgHeight / 2;
    bgScreenEndX = bgScreenStartX + scaledBgWidth;
    bgScreenEndY = bgScreenStartY + scaledBgHeight;
}
//创建商店
void GameScene::initShop() {

    auto visibleSize = Director::getInstance()->getVisibleSize();
    //获取商店原尺寸
    auto shopSize = shop->getContentSize();
    // 计算商店栏缩放比例（宽度与背景图内部区域匹配）
    float shopScale = scaledBgWidth / (shopSize.width / 0.6);
    shop->setScale(shopScale);
    // 计算缩放后的商店栏高度
    float scaledShopHeight = shopSize.height * shopScale;
    float shopY = bgScreenEndY - scaledShopHeight / 2;
    shop->setPosition(Vec2(visibleSize.width / 2, shopY));
    this->addChild(shop, 5);
    return;
}
// 初始化植物卡片
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
// 创建植物卡片
void GameScene::createPlantCard(PlantType type, const Vec2& position, float cardScale) {
    // 创建卡片背景
    auto card = Sprite::create("card.png");
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
        case PlantType::CHERRYBOMB: plantImage = "cheerybomb.png"; break;
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
            card->getContentSize().height*0.1f));
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

    // 保存卡片引用
    plantCards.pushBack(card);
    this->addChild(card, 10);
}
// 选择植物卡片
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

// 取消选择植物卡片
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
//创建网格系统
void GameScene::initGrid() {
    //背景未初始化
    if (scaledBgWidth == 0 || scaledBgHeight == 0) {
        return;
    }
    float startXPercent = 0.2f;    // 草地开始X位置百分比 
    float startYPercent = 0.074f;    // 草地开始Y位置百分比 
    float widthPercent = 0.6f;     // 草地宽度百分比 
    float heightPercent = 0.62f;    // 草地高度百分比

    // 计算草地网格的实际屏幕坐标、宽度、高度
    gridStartX = bgScreenStartX + scaledBgWidth * startXPercent;
    gridStartY = bgScreenStartY + scaledBgHeight * startYPercent;
    gridWidth = scaledBgWidth * widthPercent;
    gridHeight = scaledBgHeight * heightPercent;

    // 计算每个单元格的宽度和高度
    cellWidth = gridWidth / GRID_COLS;
    cellHeight = gridHeight / GRID_ROWS;
}
// 显示种植预览
void GameScene::showPlantingPreview() {
    if (selectedPlant == PlantType::NONE || !selectedCard) return;

    std::string plantImage;
    switch (selectedPlant) {
        case PlantType::PEASHOOTER: plantImage = "pea-shooter.png"; break;
        case PlantType::SNOWPEA: plantImage = "ice-pea-shooter.png"; break;
        case PlantType::SUNFLOWER: plantImage = "sunflower.png"; break;
        case PlantType::WALLNUT: plantImage = "nut.png"; break;
        case PlantType::FIRETREE: plantImage = "firetree.png"; break;
        case PlantType::CHERRYBOMB: plantImage = "cheerybomb.png"; break;
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

// 隐藏种植预览
void GameScene::hidePlantingPreview() {
    if (plantingPreview) {
        plantingPreview->removeFromParent();
        plantingPreview = nullptr;
    }
}

// 获取网格中心坐标
Vec2 GameScene::getGridCenter(int row, int col) {
    if (!isGridPositionValid(row, col)) {
        return Vec2::ZERO;
    }

    float x = gridStartX + col * cellWidth + cellWidth / 2;
    float y = gridStartY + row * cellHeight + cellHeight / 2;
    return Vec2(x, y);
}
// 从屏幕坐标获取网格位置
std::pair<int, int> GameScene::getGridFromPosition(const Vec2& position) {
    if (position.x < gridStartX || position.x > gridStartX + gridWidth ||
        position.y < gridStartY || position.y > gridStartY + gridHeight) {
        return std::make_pair(-1, -1); // 无效位置
    }

    int col = static_cast<int>((position.x - gridStartX) / cellWidth);
    int row = static_cast<int>((position.y - gridStartY) / cellHeight);

    col = std::max(0, std::min(col, GRID_COLS - 1));
    row = std::max(0, std::min(row, GRID_ROWS - 1));

    return std::make_pair(row, col);
}
// 检查网格位置是否有效
bool GameScene::isGridPositionValid(int row, int col) {
    return row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS;
}
// 放置植物
void GameScene::placePlant(int row, int col) {
    if (selectedPlant == PlantType::NONE || !canPlacePlant(row, col)) {
        return;
    }

    int price = Plant::getCost(selectedPlant);

    // 检查阳光是否足够
    if (sunshineCount < price) {
        unselectPlantCard();
        return;
    }

    // 扣除阳光
    updateSunshineCount(-price);

    // 创建植物
    createPlantAtGrid(selectedPlant, row, col);

    // 取消选择
    unselectPlantCard();
}

// 检查是否可以放置植物
bool GameScene::canPlacePlant(int row, int col) {
    // 检查网格是否有效
    if (!isGridPositionValid(row, col)) {
        return false;
    }

    // 检查该位置是否已经有植物
    if (gridPlants[row][col] != nullptr) {
        return false;
    }

    // 检查阳光是否足够
    if (selectedPlant != PlantType::NONE) {
        int price = Plant::getCost(selectedPlant);
        return sunshineCount >= price;
    }

    return false;
}

// 在网格创建植物
void GameScene::createPlantAtGrid(PlantType type, int row, int col) {
    std::string plantImage;
    switch (type) {
        case PlantType::PEASHOOTER: plantImage = "pea-shooter.png"; break;
        case PlantType::SNOWPEA: plantImage = "ice-pea-shooter.png"; break;
        case PlantType::SUNFLOWER: plantImage = "sunflower.png"; break;
        case PlantType::WALLNUT: plantImage = "nut.png"; break;
        case PlantType::FIRETREE: plantImage = "firetree.png"; break;
        case PlantType::CHERRYBOMB: plantImage = "cheerybomb.png"; break;
        default: return;
    }

    auto plant = Sprite::create(plantImage);
    if (!plant) return;

    // 设置植物位置和大小
    Vec2 gridCenter = getGridCenter(row, col);
    float plantScale = cellWidth / plant->getContentSize().width * 1.2f;
    plant->setScale(plantScale);
    plant->setPosition(gridCenter);

    // 添加种植动画
    plant->setScale(0.1f);
    auto scaleTo = ScaleTo::create(0.3f, plantScale);
    auto bounce = EaseBounceOut::create(scaleTo);
    plant->runAction(bounce);

    // 保存植物到网格
    gridPlants[row][col] = plant;

    // 添加植物到场景
    this->addChild(plant, 10);
}
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
        sunshineLabel->setPosition(Vec2(iconX +10, iconY -100)); // 在图标下侧显示数字
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

    float sunshineScale = cellWidth/ sunshine->getContentSize().width;
    sunshine->setPosition(position);
    sunshine->setScale(sunshineScale); 
    sunshine->setTag(100); // 设置标签，用于标识阳光

    // 添加阳光闪烁效果
    auto fadeOut = FadeTo::create(0.5f, 200);
    auto fadeIn = FadeTo::create(0.5f, 255);
    auto blink = Sequence::create(fadeOut, fadeIn, nullptr);
    sunshine->runAction(RepeatForever::create(blink));

    // 添加下落动画
    auto visibleSize = Director::getInstance()->getVisibleSize();
    float targetY = gridStartY;
    auto moveDown = MoveBy::create(5.0f / speed, Vec2(0, -(position.y - targetY)));
    auto bounce = EaseBounceOut::create(moveDown->clone());

    sunshine->runAction(Sequence::create(
        bounce,
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
        CallFunc::create([this,sunshine]() {
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

    // 暂停所有植物卡片的触摸事件
    for (auto& card : plantCards) {
        card->pause();
    }

    // 暂停所有阳光的动画
    for (auto& sunshine : sunshines) {
        sunshine->pause();
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

    // 恢复游戏
    this->resume();
    Director::getInstance()->resume();
}
// 重新开始游戏
void GameScene::restartGame() {
    // 恢复游戏（确保在场景切换前恢复）
    Director::getInstance()->resume();
    // 重新加载当前场景
    auto scene = GameScene::createScene();
    auto transition = TransitionFade::create(0.5f, scene, Color3B::BLACK);
    Director::getInstance()->replaceScene(transition);
}
// 退出到主菜单
void GameScene::exitToMainMenu() {
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