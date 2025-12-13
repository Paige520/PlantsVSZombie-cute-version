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

    // 阳光系统相关
    cocos2d::Label* sunshineLabel;
    int sunshineCount;
    cocos2d::Sprite* sunshineIcon; 
    cocos2d::Vector<cocos2d::Sprite*> sunshines;

    //菜单设置
    cocos2d::Menu* pauseMenu;                 // 暂停菜单
    cocos2d::LayerColor* pauseLayer;          // 暂停背景层
    cocos2d::Label* speedLabel;               // 速度显示标签
    float speed;                              // 游戏速度 (1.0/1.5/2.0)

    // 初始化函数
    void initBackground();
    void initShop();
    void initGrid();
    void initSunshineSystem();
    void generateSunshine();

    // 工具函数
    void calculateBackgroundInfo();
    // 坐标转换函数
    cocos2d::Vec2 getGridCenter(int row, int col);
    std::pair<int, int> getGridFromPosition(const cocos2d::Vec2& position);
    bool isGridPositionValid(int row, int col);

    // 阳光相关函数
    void createSunshine(const cocos2d::Vec2& position);
    void collectSunshine(cocos2d::Sprite* sunshine);
    void updateSunshineCount(int delta);

    //菜单相关函数
    void createPauseButton();                  // 创建暂停按钮
    void createPauseMenu();                    // 创建暂停菜单界面
    void showPauseMenu();                      // 显示暂停菜单
    void hidePauseMenu();                      // 隐藏暂停菜单
    void restartGame();                        // 重新开始游戏
    void exitToMainMenu();                     // 返回主菜单
    void changeGameSpeed();                    // 切换游戏速度
    void updateSpeedLabel();                   // 更新速度显示
};

#endif 