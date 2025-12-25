#include "Zombie.h"
#include "Plant.h"
#include "GameScene.h"

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
    // 初始化场景引用为nullptr
    m_scene = nullptr;

    // 设置僵尸基础属性
    m_type = type;
    m_state = ZombieState::MOVING;
    m_hp = getHPForType(type);
    m_originalSpeed = getSpeedForType(type);
    m_baseSpeed = m_originalSpeed;
    m_speed = m_baseSpeed;
    m_attackDamage = getAttackDamageForType(type);
    m_attackInterval = getAttackIntervalForType(type);
    m_attackTimer = 0.0f;
    m_target = nullptr;

    // 初始化减速相关属性
    m_slowFactor = 1.0f;
    m_slowDuration = 0.0f;

    // 根据僵尸类型设置具体属性
    switch (type) {
    case ZombieType::NORMAL:
        filename = "zombie.png"; // 普通僵尸
        break;
    case ZombieType::FLAG:
        filename = "flagzombie.png"; // 旗帜僵尸
        break;
    case ZombieType::BUCKET:
        filename = "ironzombie.png"; // 铁桶僵尸
        break;
    case ZombieType::CONE:
        filename = "roasbloakzombie.png"; // 路障僵尸
        break;
    }

    if (!GameObject::init(filename)) return false;

    // 设置僵尸大小
    this->setScale(0.8f);

    // 创建动画
    createAnimations();

    this->scheduleUpdate();
    return true;
}

// 设置场景引用
void Zombie::setScene(GameScene* scene) {
    m_scene = scene;
    // 设置场景后立即更新速度
    updateSpeedWithGameSpeed(m_scene ? m_scene->getGameSpeed() : 1.0f);
}

// 更新速度（考虑游戏速度）
void Zombie::updateSpeedWithGameSpeed(float gameSpeed) {
    // 根据游戏速度调整基础速度
    m_baseSpeed = m_originalSpeed * gameSpeed;

    // 如果有减速效果，应用减速因子
    if (m_slowDuration > 0.0f) {
        m_speed = m_baseSpeed * m_slowFactor;
    }
    else {
        m_speed = m_baseSpeed;
    }
}

// 获取僵尸属性的辅助方法
int Zombie::getHPForType(ZombieType type) const {
    switch (type) {
    case ZombieType::NORMAL:
    case ZombieType::FLAG:
        return 200;
    case ZombieType::CONE:
        return 560;
    case ZombieType::BUCKET:
        return 1300;
    default:
        return 200;
    }
}

float Zombie::getSpeedForType(ZombieType type) const {
    switch (type) {
    case ZombieType::NORMAL:
    case ZombieType::CONE:
    case ZombieType::BUCKET:
        return 10.0f;
    case ZombieType::FLAG:
        return 15.0f;
    default:
        return 10.0f;
    }
}

float Zombie::getAttackDamageForType(ZombieType type) const {
    switch (type) {
    case ZombieType::NORMAL:
    case ZombieType::FLAG:
    case ZombieType::CONE:
    case ZombieType::BUCKET:
        return 10.0f;
    default:
        return 10.0f;
    }
}

float Zombie::getAttackIntervalForType(ZombieType type) const {
    switch (type) {
    case ZombieType::NORMAL:
    case ZombieType::FLAG:
    case ZombieType::CONE:
    case ZombieType::BUCKET:
        return 1.5f;
    default:
        return 1.5f;
    }
}

float Zombie::getSpeed() const { return m_speed; }

void Zombie::setSpeed(float speed) {
    m_baseSpeed = speed;
    // 如果没有减速效果，更新当前速度
    if (m_slowDuration <= 0.0f) {
        m_speed = speed;
    }
}

void Zombie::setState(ZombieState state) {
    // 如果状态没有变化，不做任何操作
    if (m_state == state) return;

    // 保存旧状态
    ZombieState oldState = m_state;
    // 设置新状态
    m_state = state;

    // 根据状态转换播放相应的动画
    if (state == ZombieState::MOVING && oldState == ZombieState::ATTACKING) {
        // 从攻击状态切换到移动状态，播放移动动画
        playMoveAnimation();
    }
    else if (state == ZombieState::ATTACKING && oldState == ZombieState::MOVING) {
        // 从移动状态切换到攻击状态，播放攻击动画
        playAttackAnimation();
    }
    else if (state == ZombieState::DYING) {
        // 进入死亡状态前停止所有动作
        stopAllActions();
    }
    else if (state == ZombieState::DEAD) {
        // 已死亡状态，停止所有动作
        stopAllActions();
    }
}

void Zombie::takeDamage(int damage) {
    m_hp -= damage;
    if (m_hp < 0) m_hp = 0;

    // 受伤效果
    auto tintRed = TintTo::create(0.1f, 255, 100, 100);
    auto tintBack = TintTo::create(0.1f, 255, 255, 255);
    auto sequence = Sequence::create(tintRed, tintBack, nullptr);
    this->runAction(sequence);

    // 如果生命值为0，进入死亡状态
    if (m_hp <= 0 && m_state != ZombieState::DEAD) {
        setState(ZombieState::DYING);
        die();
    }
}

// 应用减速效果
void Zombie::applySlow(float slowFactor, float duration) {
    // 设置减速因子和持续时间
    m_slowFactor = slowFactor;
    m_slowDuration = duration;

    // 立即更新当前速度
    m_speed = m_baseSpeed * m_slowFactor;

    // 添加减速视觉效果（更鲜明的蓝色 tint）
    auto tintBlue = TintTo::create(0.1f, 0, 100, 255);
    this->runAction(tintBlue);
}

// 更新减速效果
void Zombie::updateSlow(float dt) {
    if (m_slowDuration > 0.0f) {
        // 减少减速持续时间
        m_slowDuration -= dt;

        // 如果减速效果结束
        if (m_slowDuration <= 0.0f) {
            // 恢复正常速度
            m_slowFactor = 1.0f;
            m_speed = m_baseSpeed;

            // 恢复正常颜色
            auto tintNormal = TintTo::create(0.1f, 255, 255, 255);
            this->runAction(tintNormal);
        }
    }
}

void Zombie::update(float dt) {
    // 如果已死亡，不执行任何操作
    if (isDead()) {
        setState(ZombieState::DEAD);
        return;
    }

    // 更新减速效果
    updateSlow(dt);

    // 根据当前状态执行不同的逻辑
    switch (m_state) {
    case ZombieState::MOVING: {
        // 寻找目标植物
        findTarget();

        // 如果找到目标，切换到攻击状态
        if (m_target != nullptr) {
            setState(ZombieState::ATTACKING);
        }
        else {
            // 否则继续移动
            move(dt);
        }
        break;
    }
    case ZombieState::ATTACKING: {
        // 检查目标是否仍然存在
        if (m_target == nullptr || m_target->getHp() <= 0) {
            m_target = nullptr;
            setState(ZombieState::MOVING);
        }
        else {
            // 否则执行攻击逻辑
            attack(dt);
        }
        break;
    }
    case ZombieState::DYING:
        // 死亡动画已经在die()方法中处理
        break;
    default:
        break;
    }
}

// 移动逻辑
void Zombie::move(float dt) {
    // 向左移动
    this->setPositionX(this->getPositionX() - m_speed * dt);

    // 如果僵尸超出屏幕左侧，移除僵尸
    auto visibleSize = Director::getInstance()->getVisibleSize();
    if (this->getPositionX() < -this->getContentSize().width) {
        this->setState(ZombieState::DEAD);
        this->removeFromParent();
    }
}

// 攻击逻辑
void Zombie::attack(float dt) {
    if (m_target == nullptr) {
        setState(ZombieState::MOVING);
        return;
    }

    // 播放啃咬动画
    if (m_attackTimer == 0.0f) {
        playAttackAnimation();
    }

    // 计时攻击
    m_attackTimer += dt;
    if (m_attackTimer >= m_attackInterval) {
        // 使用植物的takeDamage方法处理伤害
        m_target->takeDamage(static_cast<int>(m_attackDamage));

        // 重置攻击计时器
        m_attackTimer = 0.0f;

        // 如果目标植物死亡，寻找下一个目标
        if (m_target->isDead()) {
            m_target = nullptr;
            setState(ZombieState::MOVING);
        }
    }
}

// 创建动画
void Zombie::createAnimations() {
    // 移动动画：使用一个简单的上下浮动效果模拟行走
    m_moveAnimation = Animation::create();
    auto moveUp = MoveBy::create(0.2f, Vec2(0, 5));
    auto moveDown = MoveBy::create(0.2f, Vec2(0, -5));
    auto moveSequence = Sequence::create(moveUp, moveDown, nullptr);
    auto moveRepeat = RepeatForever::create(moveSequence);
    this->runAction(moveRepeat);

    // 攻击动画将在攻击时直接创建并播放，不需要预先创建
}

// 播放移动动画
void Zombie::playMoveAnimation() {
    // 移动动画已经在createAnimations中启动，这里只需要确保攻击动画停止
    this->stopAllActions();

    // 重新启动移动动画
    auto moveUp = MoveBy::create(0.2f, Vec2(0, 5));
    auto moveDown = MoveBy::create(0.2f, Vec2(0, -5));
    auto moveSequence = Sequence::create(moveUp, moveDown, nullptr);
    auto moveRepeat = RepeatForever::create(moveSequence);
    this->runAction(moveRepeat);
}

// 播放攻击动画
void Zombie::playAttackAnimation() {
    // 停止所有动作
    this->stopAllActions();

    // 创建啃咬动画：快速左右移动模拟啃咬
    auto biteLeft = MoveBy::create(0.1f, Vec2(10, 0));
    auto biteRight = MoveBy::create(0.1f, Vec2(-10, 0));
    auto biteLeft2 = MoveBy::create(0.1f, Vec2(10, 0));
    auto biteRight2 = MoveBy::create(0.1f, Vec2(-10, 0));
    auto biteSequence = Sequence::create(biteLeft, biteRight, biteLeft2, biteRight2, nullptr);

    // 播放啃咬动画
    this->runAction(biteSequence);
}

// 死亡逻辑
void Zombie::die() {
    // 创建死亡动画
    auto fadeOut = FadeOut::create(1.0f);
    auto scaleDown = ScaleTo::create(1.0f, 0.1f);
    auto removeSelf = RemoveSelf::create();
    auto spawn = Spawn::create(fadeOut, scaleDown, nullptr);
    auto sequence = Sequence::create(spawn, removeSelf, nullptr);
    this->runAction(sequence);

    // 设置状态为已死亡
    setState(ZombieState::DEAD);
}

// 寻找目标植物
void Zombie::findTarget() {
    // 这里需要获取场景中的所有植物并检查碰撞
    // 由于植物管理在GameScene中，这里需要通过GameScene来实现目标寻找
    // 简单实现：假设场景中有一个getPlants()方法返回所有植物
    // 注意：实际实现中，这个方法应该通过GameScene来获取植物列表并检查碰撞
    // 这里只是一个占位实现
    m_target = nullptr;
}

// 检查与植物的碰撞
bool Zombie::checkCollisionWithPlant(Plant* plant) const {
    if (plant == nullptr) return false;

    // 简单的矩形碰撞检测
    auto zombieRect = this->getBoundingBox();
    auto plantRect = plant->getBoundingBox();

    return zombieRect.intersectsRect(plantRect);
}