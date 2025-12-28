#ifndef __PLANT_H__
#define __PLANT_H__

#include "GameObject.h"
#include "cocos2d.h"

// 前向声明GameScene和Zombie类，避免循环引用
class GameScene;
class Zombie;

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

// 定义子弹类型枚举
enum class BulletType {
    NORMAL,     // 普通子弹
    ICE,        // 寒冰子弹
    FIRE        // 火焰子弹
};

class Plant : public GameObject {
public:
    // 静态创建函数，符合 Cocos2d-x 内存管理风格
    static Plant* createPlant(PlantType type);

    // 重写 update，用于处理植物逻辑（如发射子弹、产生阳光）
    virtual void update(float dt) override;
    
    // 重写基类的hp方法，兼容旧代码
    virtual void setHp(int hp) override; // 重写基类方法
    virtual int getHp() const override; // 重写基类方法

    // 获取植物类型
    PlantType getType() const { return m_type; }

    // 静态函数获取植物属性
    static int getCost(PlantType type);
    static float getCooldown(PlantType type);
    static float getAttackInterval(PlantType type);
    
    // 设置场景引用
    void setScene(GameScene* scene) { m_scene = scene; }
    
    // 设置网格位置
    void setGridPosition(int row, int col) { m_gridRow = row; m_gridCol = col; }
    
    // 获取网格位置
    int getGridRow() const { return m_gridRow; }
    int getGridCol() const { return m_gridCol; }

private:
    bool initPlant(PlantType type);
    PlantType m_type;
    GameScene* m_scene; // 场景引用，用于植物与场景交互
    int m_gridRow;     // 植物所在网格行
    int m_gridCol;     // 植物所在网格列
    
    // 植物属性
    float m_attackInterval;   // 攻击间隔
    float m_attackTimer;      // 攻击计时器
    float m_sunProductionTime; // 阳光产生时间
    float m_sunTimer;         // 阳光产生计时器
    
    // 血量相关
    int m_health;             // 当前血量
    int m_maxHealth;          // 最大血量
    // 樱桃炸弹爆炸标志
    bool m_hasExploded;
    cocos2d::ProgressTimer* m_healthBar; // 血条显示
    
    // 方法
    void shootBullet();       // 发射子弹
    void produceSunshine();   // 产生阳光
    void explode();           // 爆炸效果（樱桃炸弹）
    void showHealthBar();     // 显示血条
    void updateHealthBar();   // 更新血条

public:
    void takeDamage(int damage); // 受到伤害（公开方法，供其他类调用）
    
    // 获取植物属性的辅助方法
    float getAttackIntervalForType(PlantType type) const;
    float getSunProductionTimeForType(PlantType type) const;
    int getMaxHealthForType(PlantType type) const;
    
    // 血量相关的get/set方法
    int getHealth() const { return m_health; }
    int getMaxHealth() const { return m_maxHealth; }
    void setHealth(int health);
    void setMaxHealth(int maxHealth) { m_maxHealth = maxHealth; }
    bool isDead() const { return m_health <= 0; } // 公开方法，供其他类调用
    
    // 智能攻击相关方法
    bool hasZombieInRow();                               // 检测行内是否有僵尸
    Zombie* getClosestZombieInRange(float range);        // 获取范围内的最近僵尸
    float getAttackRange() const;                        // 获取攻击范围
    float getDistanceToZombie(Zombie* zombie) const;     // 计算到僵尸的距离
};

// 子弹类
class Bullet : public GameObject {
public:
    static Bullet* createBullet(BulletType type);
    virtual void update(float dt) override;
    virtual ~Bullet();  // 析构函数声明
    
    // 设置和获取伤害值
    void setDamage(int damage) { m_damage = damage; }
    int getDamage() const { return m_damage; }
    
    // 设置和获取速度
    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }
    
    // 设置和获取子弹类型
    void setBulletType(BulletType type) { m_type = type; }
    BulletType getBulletType() const { return m_type; }
    
    // 设置是否是穿透型子弹
    void setPiercing(bool piercing) { m_isPiercing = piercing; }
    bool isPiercing() const { return m_isPiercing; }
    
    // 设置和获取是否死亡
    void setIsDead(bool dead) { m_isDead = dead; }
    bool getIsDead() const { return m_isDead; }
    
    // 切换为火焰子弹
    void switchToFireBullet();

private:
    bool initBullet(BulletType type);
    BulletType m_type;
    int m_damage;       // 伤害值
    float m_speed;      // 速度
    bool m_isPiercing;  // 是否穿透
    bool m_isDead;      // 是否死亡
};

#endif // __PLANT_H__