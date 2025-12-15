#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#include "cocos2d.h"
#include <string>

class GameObject : public cocos2d::Sprite {
public:
    // 初始化函数
    virtual bool init(const std::string& filename);

    // 属性操作
    void setHp(int hp) { m_hp = hp; }
    int getHp() const { return m_hp; }

protected:
    int m_hp; // 生命值
};

#endif // __GAME_OBJECT_H__