#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include "GameObject.h"

enum class ZombieType {
    NORMAL,     // ÆÕÍ¨½©Ê¬
    FLAG,       // ÆìÖÄ½©Ê¬
    BUCKET,     // ÌúÍ°½©Ê¬
    CONE        // Â·ÕÏ½©Ê¬
};

class Zombie : public GameObject {
public:
    static Zombie* createZombie(ZombieType type);
    virtual void update(float dt) override;

private:
    bool initZombie(ZombieType type);
    float m_speed;
};

#endif