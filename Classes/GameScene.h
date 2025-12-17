#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Plant.h"
#include "Zombie.h"
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
    static const int GRID_ROWS = 5;
    static const int GRID_COLS = 9;
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

	// 植物种植相关
    Sprite* selectedCard;                       // 当前选中的卡片
    Vector<Sprite*> plantCards;                 // 植物卡片容器
    PlantType selectedPlant;                    // 当前选中的植物类型
    Sprite* plantingPreview;                    // 种植预览精灵
    Sprite* gridPlants[GRID_ROWS][GRID_COLS]; // 网格中的植物指针数组
    void initPlantCards();                      // 初始化植物卡片
    void createPlantCard(PlantType type, const Vec2& position,float cardScale); // 创建植物卡片
    void selectPlantCard(Sprite* card, PlantType type);         // 选择植物卡片
    void unselectPlantCard();                   // 取消选择植物卡片
    void placePlant(int row, int col);          // 放置植物
    void showPlantingPreview();                 // 显示种植预览
    void hidePlantingPreview();                 // 隐藏种植预览
    bool canPlacePlant(int row, int col);       // 检查是否可以放置植物
    void createPlantAtGrid(PlantType type, int row, int col);    // 在网格创建植物
    
    // 僵尸相关
    Vector<Zombie*> zombies;  // 存储所有僵尸
    float zombieSpawnTimer;   // 僵尸生成计时器
    float zombieSpawnInterval; // 僵尸生成间隔
    void initZombieSystem();      // 初始化僵尸系统
    void generateRandomZombie();  // 随机生成僵尸
    void updateZombies(float dt); // 更新僵尸状态
    void checkPlantZombieCollision(); // 检查植物和僵尸的碰撞
    void removeDeadZombies();     // 移除死亡的僵尸
    virtual void update(float dt) override; // 每帧更新

    //游戏结束判定
    void gameOver();
};

#endif 