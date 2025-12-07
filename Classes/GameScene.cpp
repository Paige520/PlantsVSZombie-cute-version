#include "GameScene.h"
#include <iostream>

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

    initBackground();
    initShop();
    initGrid();
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