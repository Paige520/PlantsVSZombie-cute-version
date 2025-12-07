#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"

USING_NS_CC;

class GameScene : public cocos2d::Scene {
public:
    static cocos2d::Scene* createScene();
    virtual bool init();

    CREATE_FUNC(GameScene);
private:
    // 游戏背景元素
    cocos2d::Sprite* background = Sprite::create("fightbackground.jpg");
    cocos2d::Sprite* shop= Sprite::create("shop.png");

    // 背景图信息
    float scaledBgWidth;
    float scaledBgHeight;
    float bgScreenStartX;
    float bgScreenStartY;
    float bgScreenEndX;
    float bgScreenEndY;

    // 网格系统
    const int GRID_ROWS = 5;
    const int GRID_COLS = 9;
    float gridStartX;     // 草地网格起始X坐标
    float gridStartY;     // 草地网格起始Y坐标
    float gridWidth;      // 草地网格宽度
    float gridHeight;     // 草地网格高度
    float cellWidth;      // 单元格宽度
    float cellHeight;     // 单元格高度

    // 初始化函数
    void initBackground();
    void initShop();
    void initGrid();


    // 工具函数
    void calculateBackgroundInfo();
    // 坐标转换函数
    cocos2d::Vec2 getGridCenter(int row, int col);
    std::pair<int, int> getGridFromPosition(const cocos2d::Vec2& position);
    bool isGridPositionValid(int row, int col);
};

#endif // __GAME_SCENE_H__