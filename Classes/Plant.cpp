#include "Plant.h"
#include "GameScene.h"

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
    
    // 设置植物基础属性
    m_type = type;
    m_attackInterval = getAttackIntervalForType(type);
    m_attackTimer = 0.0f;
    m_sunProductionTime = getSunProductionTimeForType(type);
    m_sunTimer = 0.0f;
    m_hasExploded = false;
    m_gridRow = -1; // 初始化为无效值
    m_gridCol = -1; // 初始化为无效值
    
    // 根据植物类型设置具体属性
    switch (type) {
        case PlantType::PEASHOOTER:
            filename = "pea-shooter.png"; // 对应普通豌豆射手
            m_maxHealth = 100;
            m_health = m_maxHealth;
            break;
        case PlantType::SUNFLOWER:
            filename = "sunflower.png";   // 对应向日葵
            m_maxHealth = 80;
            m_health = m_maxHealth;
            break;
        case PlantType::WALLNUT:
            filename = "nut.png";         // 对应坚果
            m_maxHealth = 1000;
            m_health = m_maxHealth;
            break;
        case PlantType::SNOWPEA:
            filename = "ice-pea-shooter.png"; // 对应寒冰射手
            m_maxHealth = 100;
            m_health = m_maxHealth;
            break;
        case PlantType::CHERRYBOMB:
            filename = "cherrybomb.png";  // 对应樱桃炸弹
            m_maxHealth = 1000;
            m_health = m_maxHealth;
            break;
        case PlantType::FIRETREE:
            filename = "firetree.png";    // 对应火炬树桩
            m_maxHealth = 100;
            m_health = m_maxHealth;
            break;
        default:
            CCLOG("ERROR: Unknown plant type %d", (int)type);
            return false;
    }
    
    CCLOG("INFO: Creating plant type %d with texture %s", (int)type, filename.c_str());

    // 调用基类初始化
    if (!GameObject::init(filename)) {
        return false;
    }

    this->scheduleUpdate(); // 开启 update 调度
    showHealthBar();        // 显示血条
    return true;
}

// 获取植物成本
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

// 获取植物冷却时间
float Plant::getCooldown(PlantType type) {
    switch (type) {
        case PlantType::PEASHOOTER:
            return 7.5f;
        case PlantType::SUNFLOWER:
            return 10.0f;
        case PlantType::WALLNUT:
            return 20.0f;
        case PlantType::SNOWPEA:
            return 15.0f;
        case PlantType::CHERRYBOMB:
            return 30.0f;
        case PlantType::FIRETREE:
            return 15.0f;
        default:
            return 0.0f;
    }
}

// 获取植物攻击间隔
float Plant::getAttackInterval(PlantType type) {
    switch (type) {
        case PlantType::PEASHOOTER:
            return 1.0f;
        case PlantType::SNOWPEA:
            return 1.0f;
        default:
            return 0.0f;
    }
}

// 获取攻击间隔的辅助方法
float Plant::getAttackIntervalForType(PlantType type) const {
    switch (type) {
        case PlantType::PEASHOOTER:
            return 1.0f;
        case PlantType::SNOWPEA:
            return 1.0f;
        default:
            return 0.0f;
    }
}

// 获取阳光产生时间的辅助方法
float Plant::getSunProductionTimeForType(PlantType type) const {
    switch (type) {
        case PlantType::SUNFLOWER:
            return 24.0f;
        default:
            return 0.0f;
    }
}

void Plant::update(float dt) {
    // 根据植物类型执行不同的逻辑
    switch (m_type) {
        case PlantType::PEASHOOTER:
        case PlantType::SNOWPEA: {
            // 攻击型植物：计时发射子弹
            m_attackTimer += dt;
            if (m_attackTimer >= m_attackInterval) {
                shootBullet();
                m_attackTimer = 0.0f;
            }
            break;
        }
        case PlantType::SUNFLOWER: {
            // 资源型植物：计时产生阳光
            m_sunTimer += dt;
            if (m_sunTimer >= m_sunProductionTime) {
                produceSunshine();
                m_sunTimer = 0.0f;
            }
            break;
        }
        case PlantType::CHERRYBOMB: {
            // 一次性植物：只爆炸一次
            if (!m_hasExploded) {
                explode();
                m_hasExploded = true;
            }
            break;
        }
        default:
            break;
    }
}

// 发射子弹
void Plant::shootBullet() {
    BulletType bulletType;
    
    // 根据植物类型选择子弹类型
    switch (m_type) {
        case PlantType::PEASHOOTER:
            bulletType = BulletType::NORMAL;
            break;
        case PlantType::SNOWPEA:
            bulletType = BulletType::ICE;
            break;
        default:
            return;
    }
    
    // 创建子弹
    Bullet* bullet = Bullet::createBullet(bulletType);
    if (bullet) {
        // 设置子弹位置（考虑植物的锚点、缩放和实际尺寸）
        Vec2 bulletPos = this->getPosition();
        
        // 计算植物右侧中心位置作为子弹发射点
        // 考虑植物的锚点（默认是0.5, 0.5）
        float plantRightX = bulletPos.x + (this->getContentSize().width * this->getScale() / 2);
        float plantCenterY = bulletPos.y;
        
        bulletPos = Vec2(plantRightX, plantCenterY);
        bullet->setPosition(bulletPos);
        
        // 设置子弹属性
        bullet->setDamage(20);
        bullet->setSpeed(300.0f); // 稍微提高子弹速度
        bullet->setBulletType(bulletType);
        bullet->setPiercing(false);
        
        // 添加子弹到场景
        this->getParent()->addChild(bullet, 10);
        
        // 获取GameScene引用并将子弹添加到子弹容器
        GameScene* gameScene = dynamic_cast<GameScene*>(this->getParent());
        if (gameScene) {
            gameScene->addBullet(bullet);
        }
    }
}

// 产生阳光
void Plant::produceSunshine() {
    if (m_scene) {
        // 设置阳光位置（向日葵花盘位置）
        Vec2 sunPos = this->getPosition();
        
        // 获取植物缩放后的实际大小
        float actualWidth = this->getContentSize().width * this->getScaleX();
        float actualHeight = this->getContentSize().height * this->getScaleY();
        
        // 针对不同植物类型微调阳光生成位置
        switch (m_type) {
            case PlantType::SUNFLOWER:
                // 向日葵：阳光固定在正上方
                sunPos.y += 80; // 固定在向日葵正上方生成阳光
                break;
            case PlantType::PEASHOOTER:
            case PlantType::SNOWPEA:
            case PlantType::FIRETREE:
                // 射手类植物：阳光从植物上方生成
                sunPos.y += actualHeight * 0.7f;
                break;
            case PlantType::WALLNUT:
                // 坚果：阳光从植物顶部生成
                sunPos.y += actualHeight * 0.8f;
                break;
            case PlantType::CHERRYBOMB:
                // 樱桃炸弹：阳光从植物中心上方生成
                sunPos.y += actualHeight * 0.6f;
                break;
            default:
                // 默认：阳光从植物上方生成
                sunPos.y += actualHeight * 0.6f;
                break;
        }
        
        // 使用GameScene的createSunshine方法创建阳光
        m_scene->createSunshine(sunPos);
    }
}

// 爆炸效果（樱桃炸弹）
void Plant::explode() {
    // 首先从网格中移除植物，避免悬空指针问题
    if (m_scene) {
        m_scene->removePlantFromGrid(m_gridRow, m_gridCol);
        
        // 调用GameScene中的爆炸伤害处理方法
        m_scene->handleCherryBombExplosion(m_gridRow, m_gridCol);
    }
    
    // 创建爆炸动画（这里用简单的缩放和淡出效果代替）
    auto scale = ScaleTo::create(0.3f, 1.5f);
    auto fadeOut = FadeOut::create(0.3f);
    auto removeSelf = RemoveSelf::create();
    auto sequence = Sequence::create(scale, fadeOut, removeSelf, nullptr);
    this->runAction(sequence);
}

// 获取植物最大生命值
int Plant::getMaxHealthForType(PlantType type) const {
    switch (type) {
        case PlantType::PEASHOOTER:
        case PlantType::SNOWPEA:
        case PlantType::FIRETREE:
            return 100;
        case PlantType::SUNFLOWER:
            return 80;
        case PlantType::WALLNUT:
        case PlantType::CHERRYBOMB:
            return 1000;
        default:
            return 0;
    }
}

// 设置植物生命值
void Plant::setHealth(int health) {
    m_health = std::max(0, std::min(health, m_maxHealth));
    updateHealthBar();
}

// 植物受到伤害
void Plant::takeDamage(int damage) {
    if (m_health <= 0) return;
    
    // 记录当前血量，用于计算血量减少的比例
    int oldHealth = m_health;
    
    // 减少血量
    m_health = std::max(0, m_health - damage);
    updateHealthBar();
    
    // 添加被攻击的视觉反馈效果
    // 1. 创建闪烁效果（闪白两次）
    auto blink = Blink::create(0.3f, 2);
    this->runAction(blink);
    
    // 2. 播放被攻击的音效
    if (m_scene) {
        m_scene->playSoundEffect("plant_hit.mp3");
    }
    
    // 如果植物死亡，执行消失逻辑
    if (m_health <= 0) {
        // 1. 先将血条淡出
        if (m_healthBar) {
            auto barFadeOut = FadeOut::create(0.5f);
            m_healthBar->runAction(barFadeOut);
        }
        
        // 2. 植物本体渐变为红色，然后淡出
        auto tintToRed = TintTo::create(0.5f, 255, 50, 50);
        auto fadeOut = FadeOut::create(0.8f);
        auto removeSelf = RemoveSelf::create(true);
        auto sequence = Sequence::create(tintToRed, fadeOut, removeSelf, nullptr);
        this->runAction(sequence);
        
        // 3. 播放植物死亡音效
        if (m_scene) {
            m_scene->playSoundEffect("plant_death.mp3");
        }
    }
}

// 显示血条
void Plant::showHealthBar() {
    if (!m_healthBar) {
        Size barSize(40, 5);
        Vec2 barPos(this->getContentSize().width / 2, this->getContentSize().height + 15);
        
        // 创建血条背景（使用精灵绘制代替图片资源）
        auto healthBarBg = Sprite::create();
        healthBarBg->setColor(Color3B(80, 80, 80));
        healthBarBg->setContentSize(barSize);
        healthBarBg->setPosition(barPos);
        this->addChild(healthBarBg, 10);
        
        // 创建血条进度条（使用精灵绘制代替图片资源）
        auto healthBarSprite = Sprite::create();
        healthBarSprite->setColor(Color3B(0, 255, 0));
        healthBarSprite->setContentSize(barSize);
        m_healthBar = ProgressTimer::create(healthBarSprite);
        m_healthBar->setType(ProgressTimer::Type::BAR);
        m_healthBar->setMidpoint(Vec2(0.5f, 0.5f));
        m_healthBar->setBarChangeRate(Vec2(1.0f, 0.0f));
        m_healthBar->setPosition(barPos);
        m_healthBar->setScaleX(0.8f);
        m_healthBar->setPercentage(100.0f); // 初始满血
        this->addChild(m_healthBar, 11);
    }
    
    updateHealthBar();
}

// 更新血条显示
void Plant::updateHealthBar() {
    if (m_healthBar) {
        float percentage = static_cast<float>(m_health) / m_maxHealth * 100.0f;
        m_healthBar->setPercentage(percentage);
        
        // 根据血量改变血条颜色
        if (percentage > 60) {
            m_healthBar->setColor(Color3B::GREEN);
        } else if (percentage > 30) {
            m_healthBar->setColor(Color3B::YELLOW);
        } else {
            m_healthBar->setColor(Color3B::RED);
        }
    }
}

// 重写基类的setHp方法，使用新的m_health变量
void Plant::setHp(int hp) {
    setHealth(hp); // 使用新的setHealth方法
}

// 重写基类的getHp方法，使用新的m_health变量
int Plant::getHp() const {
    return getHealth(); // 使用新的getHealth方法
}

// 子弹类实现
Bullet* Bullet::createBullet(BulletType type) {
    Bullet* pRet = new(std::nothrow) Bullet();
    if (pRet && pRet->initBullet(type)) {
        pRet->autorelease();
        return pRet;
    }
    else {
        delete pRet;
        pRet = nullptr;
        return nullptr;
    }
}

bool Bullet::initBullet(BulletType type) {
    std::string filename;
    
    // 根据子弹类型设置属性
    switch (type) {
        case BulletType::NORMAL:
            filename = "pea.png";
            break;
        case BulletType::ICE:
            filename = "icepea.png";
            break;
        case BulletType::FIRE:
            filename = "firepea.png";
            break;
        default:
            return false;
    }
    
    // 初始化子弹
    if (!GameObject::init(filename)) {
        return false;
    }
    
    m_type = type;
    m_damage = 20;
    m_speed = 200.0f;
    m_isPiercing = false;
    m_isDead = false;
    
    // 设置子弹大小（根据实际游戏需求调整缩放比例）
    this->setScale(0.1f); // 调整子弹尺寸为0.1f，符合最新设计要求
    
    // 确保子弹锚点正确，以便碰撞检测和位置计算更准确
    this->setAnchorPoint(Vec2(0.5, 0.5));
    
    // 暂时不添加粒子系统效果，避免因缺少资源文件导致闪退
    // 可以在后续添加正确的粒子系统资源文件后再启用此功能
    CCLOG("Info: Bullet particle system disabled for stability");
    
    return true;
}

Bullet::~Bullet() {
    // 确保所有子节点（特别是粒子系统）都被正确释放
    this->removeAllChildrenWithCleanup(true);
}

void Bullet::update(float dt) {
    // 子弹移动逻辑
    Vec2 pos = this->getPosition();
    pos.x += m_speed * dt;
    this->setPosition(pos);
    
    // 子弹的边界检查完全由GameScene_ZombieSysterm.cpp中的checkBulletZombieCollision函数处理
}

// 切换为火焰子弹
void Bullet::switchToFireBullet() {
    // 将子弹类型改为FIRE
    m_type = BulletType::FIRE;
    
    // 加载火焰子弹纹理
    std::string fireBulletFile = "firepea.png";
    auto fireBulletTexture = Director::getInstance()->getTextureCache()->addImage(fireBulletFile);
    if (fireBulletTexture) {
        this->setTexture(fireBulletTexture);
    } else {
        CCLOG("Warning: Failed to load fire bullet texture: %s", fireBulletFile.c_str());
    }
    
    // 火焰子弹伤害翻倍
    m_damage = m_damage * 2;
}