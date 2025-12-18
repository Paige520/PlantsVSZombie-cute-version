#include "GameScene.h"

USING_NS_CC;

//创建网格系统
void GameScene::initGrid() {
    //背景未初始化
    if (scaledBgWidth == 0 || scaledBgHeight == 0) {
        return;
    }
    float startXPercent = 0.2f;    // 草地开始X位置百分比 
    float startYPercent = 0.074f;    // 草地开始Y位置百分比 
    float widthPercent = 0.6f;     // 草地宽度百分比 
    float heightPercent = 0.62f;    // 草地高度百分比

    // 计算草地网格的实际屏幕坐标、宽度、高度
    gridStartX = bgScreenStartX + scaledBgWidth * startXPercent;
    gridStartY = bgScreenStartY + scaledBgHeight * startYPercent;
    gridWidth = scaledBgWidth * widthPercent;
    gridHeight = scaledBgHeight * heightPercent;

    // 计算每个单元格的宽度和高度
    cellWidth = gridWidth / GRID_COLS;
    cellHeight = gridHeight / GRID_ROWS;
}
// 获取网格中心坐标
Vec2 GameScene::getGridCenter(int row, int col) {
    if (!isGridPositionValid(row, col)) {
        return Vec2::ZERO;
    }

    float x = gridStartX + col * cellWidth + cellWidth / 2;
    float y = gridStartY + row * cellHeight + cellHeight / 2;
    return Vec2(x, y);
}
// 从屏幕坐标获取网格位置
std::pair<int, int> GameScene::getGridFromPosition(const Vec2& position) {
    if (position.x < gridStartX || position.x > gridStartX + gridWidth ||
        position.y < gridStartY || position.y > gridStartY + gridHeight) {
        return std::make_pair(-1, -1); // 无效位置
    }

    int col = static_cast<int>((position.x - gridStartX) / cellWidth);
    int row = static_cast<int>((position.y - gridStartY) / cellHeight);

    col = std::max(0, std::min(col, GRID_COLS - 1));
    row = std::max(0, std::min(row, GRID_ROWS - 1));

    return std::make_pair(row, col);
}
// 检查网格位置是否有效
bool GameScene::isGridPositionValid(int row, int col) {
    return row >= 0 && row < GRID_ROWS && col >= 0 && col < GRID_COLS;
}
