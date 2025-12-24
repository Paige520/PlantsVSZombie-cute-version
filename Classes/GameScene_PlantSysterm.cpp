#include "GameScene.h"
#include "Plant.h"

USING_NS_CC;

// 放置植物
void GameScene::placePlant(int row, int col) {
    if (selectedPlant == PlantType::NONE || !canPlacePlant(row, col)) {
        return;
    }

    int price = Plant::getCost(selectedPlant);

    // 检查阳光是否足够
    if (sunshineCount < price) {
        unselectPlantCard();
        return;
    }

    // 扣除阳光
    updateSunshineCount(-price);

    // 创建植物
    createPlantAtGrid(selectedPlant, row, col);

    // 启动冷却计时器
    float cooldownTime = cardCooldowns[selectedPlant];
    if (cooldownTime > 0.0f) {
        cardCooldownTimers[selectedPlant] = cooldownTime;
        
        // 显示冷却遮罩
        auto maskIter = plantTypeToCooldownMaskMap.find(selectedPlant);
        if (maskIter != plantTypeToCooldownMaskMap.end()) {
            maskIter->second->setVisible(true);
        }
    }

    // 取消选择
    unselectPlantCard();
}
// 检查是否可以放置植物
bool GameScene::canPlacePlant(int row, int col) {
    // 检查网格是否有效
    if (!isGridPositionValid(row, col)) {
        return false;
    }

    // 检查该位置是否已经有植物
    if (gridPlants[row][col] != nullptr) {
        return false;
    }

    // 检查阳光是否足够
    if (selectedPlant != PlantType::NONE) {
        int price = Plant::getCost(selectedPlant);
        return sunshineCount >= price;
    }

    return false;
}
// 在网格创建植物
void GameScene::createPlantAtGrid(PlantType type, int row, int col) {
    CCLOG("INFO: createPlantAtGrid called with type %d, row %d, col %d", (int)type, row, col);
    
    // 使用Plant::createPlant()方法创建植物实例
    auto plant = Plant::createPlant(type);
    if (!plant) {
        CCLOG("ERROR: Failed to create plant instance for type %d", (int)type);
        return;
    }
    
    CCLOG("INFO: Plant instance created successfully");

    // 设置植物位置和大小
    Vec2 gridCenter = getGridCenter(row, col);
    CCLOG("INFO: Grid center at (%.2f, %.2f)", gridCenter.x, gridCenter.y);
    
    Size plantSize = plant->getContentSize();
    CCLOG("INFO: Plant content size (%.2f, %.2f)", plantSize.width, plantSize.height);
    
    float plantScale = cellWidth / plantSize.height * 1.2f;
    CCLOG("INFO: Plant scale factor %.2f", plantScale);
    
    plant->setScale(plantScale);
    plant->setPosition(gridCenter);

    // 添加种植动画
    plant->setScale(0.1f);
    auto scaleTo = ScaleTo::create(0.3f, plantScale);
    auto bounce = EaseBounceOut::create(scaleTo);
    plant->runAction(bounce);

    // 保存植物到网格
    gridPlants[row][col] = plant;
    CCLOG("INFO: Plant saved to grid at row %d, col %d", row, col);

    // 添加植物到场景
    this->addChild(plant, 10);
    CCLOG("INFO: Plant added to scene");

    // 设置植物的场景引用，以便植物可以与场景交互（如产生阳光、发射子弹等）
    plant->setScene(this);
    
    // 设置植物的网格位置
    plant->setGridPosition(row, col);
    
    CCLOG("INFO: Plant scene reference set");
}

// 从网格移除植物
void GameScene::removePlantFromGrid(int row, int col) {
    if (isGridPositionValid(row, col)) {
        // 从网格中移除植物指针
        gridPlants[row][col] = nullptr;
        CCLOG("INFO: Plant removed from grid at row %d, col %d", row, col);
    }
}