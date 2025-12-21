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
    std::string plantImage;
    switch (type) {
    case PlantType::PEASHOOTER: plantImage = "pea-shooter.png"; break;
    case PlantType::SNOWPEA: plantImage = "ice-pea-shooter.png"; break;
    case PlantType::SUNFLOWER: plantImage = "sunflower.png"; break;
    case PlantType::WALLNUT: plantImage = "nut.png"; break;
    case PlantType::FIRETREE: plantImage = "firetree.png"; break;
    case PlantType::CHERRYBOMB: plantImage = "cherrybomb.png"; break;
    default: return;
    }

    auto plant = Sprite::create(plantImage);
    if (!plant) return;

    // 设置植物位置和大小
    Vec2 gridCenter = getGridCenter(row, col);
    float plantScale = cellWidth / plant->getContentSize().width * 1.2f;
    plant->setScale(plantScale);
    plant->setPosition(gridCenter);

    // 添加种植动画
    plant->setScale(0.1f);
    auto scaleTo = ScaleTo::create(0.3f, plantScale);
    auto bounce = EaseBounceOut::create(scaleTo);
    plant->runAction(bounce);

    // 保存植物到网格
    gridPlants[row][col] = plant;

    // 添加植物到场景
    this->addChild(plant, 10);
}
