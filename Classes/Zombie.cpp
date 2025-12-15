#include "Zombie.h"

using namespace cocos2d;

Zombie* Zombie::createZombie(ZombieType type) {
    Zombie* pRet = new(std::nothrow) Zombie();
    if (pRet && pRet->initZombie(type)) {
        pRet->autorelease();
        return pRet;
    }
    delete pRet;
    return nullptr;
}

bool Zombie::initZombie(ZombieType type) {
    std::string filename;
    int hp = 0;

    switch (type) {
        case ZombieType::NORMAL:
            filename = "zombie.png"; // 普通僵尸素材
            hp = 200;
            break;
        case ZombieType::FLAG:
            filename = "flagzombie.png"; // 旗帜僵尸素材
            hp = 200;
            break;
        case ZombieType::BUCKET:
            filename = "ironzombie.png"; // 铁桶僵尸素材
            hp = 1300; // 铁桶血量高
            break;
        case ZombieType::CONE:
            filename = "roasbloakzombie.png"; // 这个文件名看起来是路障僵尸
            hp = 560; // 路障血量中等
            break;
    }

    if (!GameObject::init(filename)) return false;

    m_hp = hp;
    m_speed = 10.0f; // 移动速度
    this->scheduleUpdate();
    return true;
}

void Zombie::update(float dt) {
    // 简单的移动逻辑：向左移动
    this->setPositionX(this->getPositionX() - m_speed * dt);
}