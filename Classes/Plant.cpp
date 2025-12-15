#include "Plant.h"

using namespace cocos2d;

Plant* Plant::createPlant(PlantType type) {
    Plant* pRet = new(std::nothrow) Plant();
    if (pRet && pRet->initPlant(type)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool Plant::initPlant(PlantType type) {
    std::string filename;
    switch (type) {
        case PlantType::PEASHOOTER:
            filename = "pea-shooter.png"; // 对应普通豌豆射手
            m_hp = 100;
            break;
        case PlantType::SUNFLOWER:
            filename = "sunflower.png";   // 对应向日葵
            m_hp = 80;
            break;
        case PlantType::WALLNUT:
            filename = "nut.png";         // 对应坚果
            m_hp = 1000;
            break;
        case PlantType::SNOWPEA:
            filename = "ice-pea-shooter.png"; // 对应寒冰射手
            m_hp = 100;
            break;
        case PlantType::CHERRYBOMB:
            filename = "cherrybomb.png";  // 对应樱桃炸弹
            m_hp = 1000;
            break;
        case PlantType::FIRETREE:
            filename = "firetree.png";    // 对应火炬树桩
            m_hp = 100;
            break;
        default:
            return false;
    }

    // 调用基类初始化
    if (!GameObject::init(filename)) {
        return false;
    }

    m_type = type;
    this->scheduleUpdate(); // 开启 update 调度
    return true;
}
int Plant::getCost(PlantType type) {
    switch (type) {
        case PlantType::PEASHOOTER:
            return 100;
        case PlantType::SUNFLOWER:
            return 50;
        case PlantType::WALLNUT:
            return 50;
        case PlantType::SNOWPEA:
            return 175;
        case PlantType::CHERRYBOMB:
            return 150;
        case PlantType::FIRETREE:
            return 175;
        default:
            return 0;
    }
}
void Plant::update(float dt) {
    // 在这里写植物的逻辑，例如：
    // 如果是向日葵，计时产生 "sunshine.png"
    // 如果是射手，计时发射 "pea.png" 或 "icepea.png"
}