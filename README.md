# PlantsVSZombie-cute-version

期末项目



\## 项目概述

本项目是一个基于Cocos2d-x引擎开发的《植物大战僵尸》简易简易版本，实现了游戏的核心玩法，包括植物种植、阳光系统、僵尸生成与攻击、游戏暂停菜单等功能。



\## 主要功能模块



\### 1. 场景管理

\- \*\*StartScene\*\*: 游戏启动场景，作为游戏入口

\- \*\*SelectScene\*\*: 选择游戏模式或退出

\- \*\*GameScene\*\*: 主游戏场景，包含所有游戏逻辑实现



\### 2. 核心游戏系统



\#### 网格系统

\- 实现了5行9列的游戏网格布局

\- 提供网格坐标与屏幕坐标的转换功能

\- 网格位置有效性检查



\#### 植物系统

\- 支持多种植物类型(豌豆射手、向日葵、坚果墙等)

\- 植物卡片选择与种植机制

\- 种植预览与有效性检查

\- 植物放置成本与阳光消耗系统



\#### 阳光系统

\- 阳光自动生成机制

\- 阳光收集与数量更新

\- 阳光显示UI



\#### 僵尸系统

\- 多种僵尸类型(普通僵尸、路障僵尸、铁桶僵尸等)

\- 随机生成僵尸逻辑

\- 僵尸移动与攻击行为

\- 僵尸与植物的碰撞检测



\#### 游戏控制

\- 暂停/继续游戏功能

\- 游戏速度调节(1.0x/1.5x/2.0x)

\- 重新开始与返回主菜单

\- 游戏结束判定与处理



\## 代码结构说明



\### 主要文件功能



1\. \*\*GameScene.h/.cpp\*\*: 游戏主场景类，包含游戏初始化和主循环

2\. \*\*GameScene\_Background.cpp\*\*: 背景初始化与计算

3\. \*\*GameScene\_GirdSysterm.cpp\*\*: 网格系统实现

4\. \*\*GameScene\_PlantSysterm.cpp\*\*: 植物种植相关逻辑

5\. \*\*GameScene\_PlantCards.cpp\*\*: 植物卡片UI与交互

6\. \*\*GameScene\_Sunshine.cpp\*\*: 阳光系统实现

7\. \*\*GameScene\_ZombieSysterm.cpp\*\*: 僵尸生成与行为逻辑

8\. \*\*GameScene\_LawnMower.cpp\*\*: 草坪修剪机功能

9\. \*\*GameScene\_PauseMenu.cpp\*\*: 暂停菜单与游戏控制

10\. \*\*StartScene.h/.cpp\*\*: 开始场景

11\. \*\*AppDelegate.h/.cpp\*\*: 应用程序入口与配置



\### 关键类与方法



\- \*\*GameScene\*\*: 主游戏场景类，整合所有游戏系统

&nbsp; - `init()`: 初始化游戏场景

&nbsp; - `update(float dt)`: 每帧更新游戏状态

&nbsp; - `placePlant(int row, int col)`: 放置植物

&nbsp; - `generateRandomZombie()`: 随机生成僵尸

&nbsp; - `checkPlantZombieCollision()`: 检查植物与僵尸碰撞



\- \*\*植物系统\*\*

&nbsp; - `initPlantCards()`: 初始化植物卡片

&nbsp; - `selectPlantCard()`: 选择植物卡片

&nbsp; - `canPlacePlant()`: 检查是否可以放置植物



\- \*\*僵尸系统\*\*

&nbsp; - `initZombieSystem()`: 初始化僵尸系统

&nbsp; - `updateZombies()`: 更新僵尸状态

&nbsp; - `removeDeadZombies()`: 移除死亡僵尸



\## 游戏操作说明



1\. 点击植物卡片选择要种植的植物(需足够阳光)

2\. 在草地上点击放置选中的植物

3\. 向日葵会产生阳光，点击收集阳光

4\. 僵尸会从右侧出现并向左移动，植物会自动攻击僵尸

5\. 点击右上角暂停按钮可以暂停游戏、调整速度或返回主菜单

6\. 当僵尸到达屏幕左侧时，游戏结束



\## 注意事项



\- 游戏资源文件(图片、音频等)需放置在正确路径

\- 本项目基于Cocos2d-x引擎开发，需配置相应开发环境



