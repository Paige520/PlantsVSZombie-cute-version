#include "GameObject.h"

bool GameObject::init(const std::string& filename) {
    // 调用父类 Sprite 的 init
    if (!Sprite::initWithFile(filename)) {
        return false;
    }
    return true;
}