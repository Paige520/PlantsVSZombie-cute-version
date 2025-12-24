#include "GameScene.h"

USING_NS_CC;

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
    if (!shop) {
        shop = Sprite::create("Shop.png");
        if (!shop) {
            CCLOG("ERROR: Failed to load Shop.png");
            return;
        }
    }

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
