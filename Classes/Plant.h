#ifndef __PLANT_H__
#define __PLANT_H__

#include "GameObject.h"

// 定义植物类型枚举
enum class PlantType {
    NONE,
    PEASHOOTER, // 豌豆射手
    SUNFLOWER,  // 向日葵
    WALLNUT,    // 坚果
    SNOWPEA,    // 寒冰射手
    CHERRYBOMB, // 樱桃炸弹
    FIRETREE    // 火炬树桩
};

class Plant : public GameObject {
public:
    // 静态创建函数，符合 Cocos2d-x 内存管理风格
    static Plant* createPlant(PlantType type);

    // 重写 update，用于处理植物逻辑（如发射子弹、产生阳光）
    virtual void update(float dt) override;

    static int getCost(PlantType type);

private:
    bool initPlant(PlantType type);
    PlantType m_type;
};

#endif // __PLANT_H__