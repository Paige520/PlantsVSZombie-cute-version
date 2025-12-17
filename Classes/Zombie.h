#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include "GameObject.h"

enum class ZombieType {
    NORMAL,     // 普通僵尸
    FLAG,       // 旗帜僵尸
    BUCKET,     // 铁桶僵尸
    CONE        // 路障僵尸
};

class Zombie : public GameObject {
public:
    static Zombie* createZombie(ZombieType type);
    virtual void update(float dt) override; // 每帧更新

    // 获取和设置生命值
    int getHP() const { return getHp(); }
    void setHP(int hp) { setHp(hp); }

    // 获取和设置速度
    float getSpeed() const;
    void setSpeed(float speed);

    // 减少生命值（被攻击时调用）
    void takeDamage(int damage);

private:
    bool initZombie(ZombieType type);
    float m_speed;
};

#endif