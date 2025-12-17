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
            filename = "zombie.png"; // 普通僵尸
            hp = 200;
            break;
        case ZombieType::FLAG:
            filename = "flagzombie.png"; // 旗帜僵尸
            hp = 200;
            break;
        case ZombieType::BUCKET:
            filename = "ironzombie.png"; // 铁桶僵尸
            hp = 1300; // 铁桶血量高
            break;
        case ZombieType::CONE:
            filename = "roasbloakzombie.png"; // 路障僵尸
            hp = 560; // 路障血量中等
            break;
    }

    if (!GameObject::init(filename)) return false;

    m_hp = hp;
    m_speed = 10.0f; // 移动速度
    this->scheduleUpdate();
    return true;
}
float Zombie::getSpeed() const { return m_speed; }
void Zombie::setSpeed(float speed) { m_speed = speed; }
void Zombie::takeDamage(int damage) {
        m_hp -= damage;
        if (m_hp < 0) m_hp = 0;

        // 受伤效果
        auto tintRed = TintTo::create(0.1f, 255, 100, 100);
        auto tintBack = TintTo::create(0.1f, 255, 255, 255);
        auto sequence = Sequence::create(tintRed, tintBack, nullptr);
        this->runAction(sequence);
}

void Zombie::update(float dt) {
    // 简单的移动逻辑：向左移动
    this->setPositionX(this->getPositionX() - m_speed * dt);
}