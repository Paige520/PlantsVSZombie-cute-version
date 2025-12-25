#ifndef __ZOMBIE_H__
#define __ZOMBIE_H__

#include "GameObject.h"
#include "cocos2d.h"

class GameScene;
// 僵尸状态枚举
enum class ZombieState {
    MOVING,     // 移动状态
    ATTACKING,  // 攻击状态
    DYING,      // 死亡状态
    DEAD        // 已死亡
};

enum class ZombieType {
    NORMAL,     // 普通僵尸
    FLAG,       // 旗帜僵尸
    BUCKET,     // 铁桶僵尸
    CONE        // 路障僵尸
};

class Plant;

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
    float getBaseSpeed() const { return m_baseSpeed; }

    // 减速相关方法
    void applySlow(float slowFactor, float duration);
    void updateSlow(float dt);
    bool isSlowed() const { return m_slowDuration > 0.0f; }

    // 获取和设置僵尸类型
    ZombieType getType() const { return m_type; }
    void setType(ZombieType type) { m_type = type; }

    // 获取和设置僵尸状态
    ZombieState getState() const { return m_state; }
    void setState(ZombieState state);

    // 减少生命值（被攻击时调用）
    void takeDamage(int damage);

    // 检查是否已死亡
    bool isDead() const { return m_hp <= 0 || m_state == ZombieState::DEAD; }

    // 获取和设置目标植物
    void setTarget(Plant* plant) { m_target = plant; }
    Plant* getTarget() const { return m_target; }

    // 设置场景引用
    void setScene(GameScene* scene);
    // 更新速度（考虑游戏速度）
    void updateSpeedWithGameSpeed(float gameSpeed);
private:
    bool initZombie(ZombieType type);

    // 僵尸属性
    ZombieType m_type;
    ZombieState m_state;
    GameScene* m_scene; // 场景引用，用于获取游戏速度
    float m_originalSpeed; // 原始速度（不考虑游戏速度）
    float m_baseSpeed;      // 基础速度
    float m_speed;          // 当前速度
    float m_attackDamage;   // 攻击力
    float m_attackInterval; // 攻击间隔
    float m_attackTimer;    // 攻击计时器

    // 减速相关属性
    float m_slowFactor;     // 减速因子 (0.0-1.0)
    float m_slowDuration;   // 减速持续时间

    // 动画相关
    cocos2d::Animation* m_moveAnimation;    // 移动动画
    cocos2d::Animation* m_attackAnimation;  // 攻击动画

    // 植物相关
    Plant* m_target;        // 当前攻击目标

    // 方法
    void move(float dt);    // 移动逻辑
    void attack(float dt);  // 攻击逻辑
    void die();             // 死亡逻辑
    void findTarget();      // 寻找目标植物
    bool checkCollisionWithPlant(Plant* plant) const; // 检查与植物的碰撞

    // 动画相关方法
    void createAnimations(); // 创建动画
    void playMoveAnimation(); // 播放移动动画
    void playAttackAnimation(); // 播放攻击动画

    // 获取僵尸属性的辅助方法
    int getHPForType(ZombieType type) const;
    float getSpeedForType(ZombieType type) const;
    float getAttackDamageForType(ZombieType type) const;
    float getAttackIntervalForType(ZombieType type) const;
};

#endif