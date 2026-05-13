#include "gamecontroller.h"
#include "player.h"
#include "playerview.h"
#include "wall.h"
#include "healthview.h"
#include "item.h"
#include "itemview.h"
#include "storyfragment.h"

#include <QKeyEvent>
#include <QDebug>
#include <QTime>
#include <QMessageBox>
#include <QtMath>
#include <QSet>
#include <QPainter>
#include <QPen>
#include <algorithm>
#include <cstdlib>

// 全局随机数种子
static bool randomSeeded = false;

// 在.cpp文件中定义常量
const int GameController::DOOR_SCORE_THRESHOLD = 612;
const int GameController::COLLISION_COOLDOWN_FRAMES = 10;

GameController::GameController(QObject *parent) : QObject(parent)
{
    if (!randomSeeded) {
        qsrand(QTime::currentTime().msec());
        randomSeeded = true;
    }

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &GameController::updateGame);
    qDebug() << "游戏控制器创建";
    initializeStories();
}

GameController::~GameController()
{
    clearItems();
    for (StoryFragment* fragment : storyFragments) {
        delete fragment;
    }
    storyFragments.clear();
    qDebug() << "游戏控制器销毁";
}

void GameController::initialize(Player* p, PlayerView* pv, Wall* w,
                               HealthView* hv, ItemView* iv)
{
    player = p;
    playerView = pv;
    wall = w;
    healthView = hv;
    itemView = iv;

    // 设置边界为迷宫的实际大小
    if (wall) {
        minX = 0;
        minY = 0;
        maxX = wall->getCols() * wall->getCellSize();  // 61 * 20 = 1220
        maxY = wall->getRows() * wall->getCellSize();  // 61 * 20 = 1220

        qDebug() << "迷宫边界设置: " << minX << "," << minY << " 到 " << maxX << "," << maxY;
    }

    if (player && healthView) {
        connect(player, &Player::healthChanged,
                healthView, &HealthView::updateDisplay);
    }

    // 生成初始物品
    generateItems(8);

    qDebug() << "游戏控制器初始化完成";
    qDebug() << "目标: 收集612分";
}

void GameController::startGame()
{
    if (gameState != Running) {
        gameTimer->start(1000 / frameRate);
        gameState = Running;
        emit gameStarted();
        qDebug() << "游戏开始";
    }
}

void GameController::stopGame()
{
    if (gameTimer && gameTimer->isActive()) {
        gameTimer->stop();
    }
    gameState = Stopped;
    emit gameStopped();
    qDebug() << "游戏停止";
}

void GameController::pauseGame()
{
    if (gameState == Running) {
        gameTimer->stop();
        gameState = Paused;
        emit gamePaused();
        qDebug() << "游戏暂停";
    }
}

void GameController::resumeGame()
{
    if (gameState == Paused) {
        gameTimer->start(1000 / frameRate);
        gameState = Running;
        emit gameResumed();
        qDebug() << "游戏继续";
    }
}

void GameController::handleKeyPress(QKeyEvent* event)
{
    if (!player || gameState != Running) return;

    switch (event->key()) {
    case Qt::Key_W:
    case Qt::Key_Up:
        player->moveUp();
        break;
    case Qt::Key_A:
    case Qt::Key_Left:
        player->moveLeft();
        break;
    case Qt::Key_S:
    case Qt::Key_Down:
        player->moveDown();
        break;
    case Qt::Key_D:
    case Qt::Key_Right:
        player->moveRight();
        break;
    case Qt::Key_Space:
        if (gameState == Running) pauseGame();
        else if (gameState == Paused) resumeGame();
        break;
    case Qt::Key_Escape:
        stopGame();
        break;
    case Qt::Key_C:
        if (currentItem && currentItem->isShowPrompt()) {
            collectCurrentItem();
        }
        break;
    case Qt::Key_R:
        if (currentItem && currentItem->isShowPrompt()) {
            readCurrentItemStory();
        }
        break;

    }
}

void GameController::handleKeyRelease(QKeyEvent* event)
{
    if (!player) return;
    switch (event->key()) {
    case Qt::Key_W: case Qt::Key_Up:
    case Qt::Key_S: case Qt::Key_Down:
    case Qt::Key_A: case Qt::Key_Left:
    case Qt::Key_D: case Qt::Key_Right:
        player->stop();
        break;
    default:
        break;
    }
}

GameController::GameState GameController::getGameState() const
{
    return gameState;
}

int GameController::getTotalScore() const
{
    return totalScore;
}

int GameController::getItemsCollected() const
{
    return itemsCollected;
}

int GameController::getRandomInt(int min, int max)
{
    if (min >= max) return min;
    return min + qrand() % (max - min + 1);
}

Item* GameController::getCurrentItem() const
{
    return currentItem;
}

QString GameController::getCurrentStory() const
{
    return currentStory;
}

void GameController::clearItems()
{
    for (Item* item : items) {
        if (item) {
            delete item;
        }
    }
    items.clear();
    currentItem = nullptr;
    qDebug() << "已清空所有物品";
}

void GameController::collectCurrentItem()
{
    // === 1. 基础检查 ===
    if (!currentItem || currentItem->isCollected()) {
        qDebug() << "[GameController] 收集物品失败: 物品无效或已收集";
        return;
    }

    // === 2. 执行收集 ===
    currentItem->setCollected(true);
    int score = currentItem->getScore();
    totalScore += score;
    itemsCollected++;

    emit itemCollected(score, totalScore);
    currentItem->setShowPrompt(false);

    if (totalScore >= DOOR_SCORE_THRESHOLD) {
        qDebug() << "★★★★★★★★★★★★★★★★★★★★★★★★";
        qDebug() << "★ 分数达标！触发最终结局。 ★";
        qDebug() << "★ 总收集物品:" << itemsCollected << "个 ★";
        qDebug() << "★ 最终分数:" << totalScore << "分 ★";
        qDebug() << "★★★★★★★★★★★★★★★★★★★★★★★★";

        // 发射最终故事信号
         emit finalStoryTriggered();

        // 游戏胜利，停止游戏循环
        emit gameWon();
        stopGame();
        return; // 重要：触发结局后，直接返回，不再执行后续的 currentItem = nullptr 等清理
    }

    // === 5. 后续清理 ===
    currentItem = nullptr;
    if (itemView) itemView->setItems(items);
    if (healthView) healthView->updateDisplay();

    qDebug() << "[GameController] collectCurrentItem 函数结束";
}

void GameController::readCurrentItemStory()
{
    if (!currentItem || currentItem->isCollected()) return;

    int itemType = currentItem->getType();
    if (itemType >= 0 && itemType < storyFragments.size()) {
        StoryFragment* fragment = storyFragments[itemType];
        if (!fragment->isRead()) {
            fragment->setRead(true);
            emit showStoryFragmentView(fragment->getContent(), fragment->getImagePath());
            qDebug() << "阅读故事碎片" << itemType + 1;
        }
    }
}

void GameController::showStoryFragment(int itemType)
{
    if (itemType >= 0 && itemType < storyFragments.size()) {
        StoryFragment* fragment = storyFragments[itemType];
        currentStory = fragment->getContent();
        emit showStoryFragmentView(fragment->getContent(), fragment->getImagePath());
    }
}

void GameController::updateGame()
{
    if (!player || gameState != Running) return;

    processPlayerMovement();
    checkBoundaries();
    if (wall) processWallCollision();
    processItemCollision();
}

void GameController::processPlayerMovement()
{
    if (!player) return;

    if (player->isMoving()) {
        player->updatePosition();
    }
}

void GameController::checkBoundaries()
{
    if (!player) return;

    QPointF pos = player->getPosition();
    float width = player->getWidth();
    float height = player->getHeight();
    float newX = pos.x();
    float newY = pos.y();

    if (pos.x() - width/2 < minX) {
        newX = minX + width/2;
    } else if (pos.x() + width/2 > maxX) {
        newX = maxX - width/2;
    }

    if (pos.y() - height/2 < minY) {
        newY = minY + height/2;
    } else if (pos.y() + height/2 > maxY) {
        newY = maxY - height/2;
    }

    if (newX != pos.x() || newY != pos.y()) {
        player->setPosition(newX, newY);
    }
}

void GameController::processWallCollision()
{
    if (!player || !wall) return;

    if (collisionCooldown > 0) {
        collisionCooldown--;
        return;
    }

    QRectF playerRect = player->getBoundingBox();
    QVector<QRectF> wallRects = wall->getWallRects();
    bool collisionOccurred = false;

    for (const QRectF& wallRect : wallRects) {
        if (playerRect.intersects(wallRect)) {
            collisionOccurred = true;

            float overlapLeft = playerRect.right() - wallRect.left();
            float overlapRight = wallRect.right() - playerRect.left();
            float overlapTop = playerRect.bottom() - wallRect.top();
            float overlapBottom = wallRect.bottom() - playerRect.top();
            float minOverlap = qMin(qMin(overlapLeft, overlapRight),
                                   qMin(overlapTop, overlapBottom));

            if (minOverlap == overlapLeft) {
                player->setPosition(player->getPosition().x() - minOverlap,
                                   player->getPosition().y());
            } else if (minOverlap == overlapRight) {
                player->setPosition(player->getPosition().x() + minOverlap,
                                   player->getPosition().y());
            } else if (minOverlap == overlapTop) {
                player->setPosition(player->getPosition().x(),
                                   player->getPosition().y() - minOverlap);
            } else if (minOverlap == overlapBottom) {
                player->setPosition(player->getPosition().x(),
                                   player->getPosition().y() + minOverlap);
            }
            break;
        }
    }

    if (collisionOccurred) {
        collisionCooldown = COLLISION_COOLDOWN_FRAMES;
        emit collisionDetected();

        int damage = wall->getDamage();
        player->takeDamage(damage);
        qDebug() << "撞墙扣血:" << damage << "点，剩余血量:" << player->getHealth();

        if (player->isDead()) {
            qDebug() << "玩家死亡！";
            stopGame();
        }
    }
}

void GameController::processItemCollision()
{
    if (!player || items.isEmpty()) return;

    if (currentItem && currentItem->isShowPrompt()) {
        currentItem->setShowPrompt(false);
    }

    Item* closestItem = nullptr;
    float closestDistance = 1000.0f;

    for (Item* item : items) {
        if (item && !item->isCollected()) {
            if (player->getBoundingBox().intersects(item->getBoundingBox())) {
                QPointF playerPos = player->getPosition();
                QPointF itemPos = item->getPosition();
                // 修复距离计算：使用欧几里得距离
                float dx = playerPos.x() - itemPos.x();
                float dy = playerPos.y() - itemPos.y();
                float distance = qSqrt(dx*dx + dy*dy);

                if (distance < closestDistance) {
                    closestDistance = distance;
                    closestItem = item;
                }
            }
        }
    }

    if (closestItem) {
        currentItem = closestItem;
        currentItem->setShowPrompt(true);
        emit showItemPrompt(currentItem);
    } else {
        currentItem = nullptr;
    }

    if (itemView) {
        itemView->setItems(items);
    }
}


void GameController::initializeStories()
{
    // 定义图片资源路径，与8个物品类型一一对应
    QString storyImagePaths[8] = {
        ":/new/prefix1/story01.png",
        ":/new/prefix1/story02.png",
        ":/new/prefix1/story03.png",
        ":/new/prefix1/story04.png",
        ":/new/prefix1/story05.png",
        ":/new/prefix1/story06.png",
        ":/new/prefix1/story07.png",
        ":/new/prefix1/story08.png",
    };

    for (int i = 0; i < 8; ++i) {
        StoryFragment* fragment = new StoryFragment(this);
        QString story = QString("这是第%1个故事碎片\n\n").arg(i + 1);

        // 【请将以下case 0到case 7的占位文本替换成您自己的小王子故事文本】
        switch (i) {
        case 0:
            story +="收集到了一条金色的蛇\n" " \n""就是它咬了我\n""不痛，像被针扎了一下\n""然后我就来到了这里\n""我是谁？这是哪里？";
            break;
        case 1:
            story += "收集到了一幅画\n"" \n""我记得它的作者是地球上的一个飞行员，\n""我们在地球的沙漠中相遇，\n""画里是蟒蛇吞下了一头大象。";
            break;
        case 2:
            story += "收集到了一棵猴面包树\n"" \n""我记得这个植物！\n""它很危险,必须每天清理。\n""对了，这是我星球上的植物，\n""我的星球很小，有三座火山\n""我是来自B-612星球的小王子！";
            break;
        case 3:
            story += "收集到了王冠和权杖\n"" \n""这应该来自我去过的一颗星球。\n""星球有一位国王，这大概是他的物品\n""他统治着一切。\n""不，他只统治他自己。\n""审判自己比审判别人难得多。";
            break;
        case 4:
            story +="收集到了一张账单\n" " \n""四亿九千九百九十九万九千九百九十一颗星星\n""他把星星锁在抽屉里，说拥有了它们。\n""但拥有到底是什么？\n""我想起了我的玫瑰...";
            break;
        case 5:
            story += "收集到了一个路灯\n"" \n""这应该来自于我去过的一颗星球。\n""这个星球每分钟转一圈。\n""星球上有一个点灯人，他没有休息日，\n""但他点亮了某样东西。";
            break;
        case 6:
            story += "收集到了一棵麦穗\n"" \n""“重要的东西用眼睛是看不见的，\n""要用心去看。”\n""我想起来了，是狐狸教会了我。";
            break;
        case 7:
            story += "收集到了玻璃罩与玫瑰\n"" \n""她总是怕风，\n""所以，我给她做了一个玻璃罩。\n""她是我独一无二的玫瑰。\n""我要回去，回到我的玫瑰身边！";
            break;
        }

        fragment->setContent(story);
        // +++ 关键：为每个故事碎片数据模型设置对应的图片路径 +++
        fragment->setImagePath(storyImagePaths[i]);

        storyFragments.append(fragment);
    }
}

QPointF GameController::getRandomValidPosition()
{
    if (!wall) {
        return QPointF(100, 100);
    }

    int rows = wall->getRows();
    int cols = wall->getCols();
    int cellSize = wall->getCellSize();

    int maxAttempts = 1000;

    for (int attempt = 0; attempt < maxAttempts; ++attempt) {
        int row = getRandomInt(0, rows - 1);
        int col = getRandomInt(0, cols - 1);

        if (wall->getCellValue(row, col) == 0) {
            float x = col * cellSize + cellSize / 2.0f;
            float y = row * cellSize + cellSize / 2.0f;

            if (x >= 15 && x <= maxX - 15 &&
                y >= 15 && y <= maxY - 15) {
                return QPointF(x, y);
            }
        }
    }

    return QPointF(100, 100);
}

bool GameController::isValidPosition(const QPointF& pos) const
{
    if (!wall) {
        return false;
    }

    if (pos.x() < 15 || pos.x() > maxX - 15 ||
        pos.y() < 15 || pos.y() > maxY - 15) {
        return false;
    }

    QRectF itemRect(pos.x() - 15, pos.y() - 15, 30, 30);
    QVector<QRectF> wallRects = wall->getWallRects();

    for (const QRectF& wallRect : wallRects) {
        if (itemRect.intersects(wallRect)) {
            return false;
        }
    }

    return true;
}

bool GameController::is3x3Passage(int centerRow, int centerCol) const
{
    if (!wall) return false;

    int rows = wall->getRows();
    int cols = wall->getCols();

    for (int r = -1; r <= 1; ++r) {
        for (int c = -1; c <= 1; ++c) {
            int checkRow = centerRow + r;
            int checkCol = centerCol + c;

            if (checkRow < 0 || checkRow >= rows ||
                checkCol < 0 || checkCol >= cols) {
                return false;
            }

            if (wall->getCellValue(checkRow, checkCol) != 0) {
                return false;
            }
        }
    }

    return true;
}

// 检查2x2区域是否都是通道（0） - 对应40x40通道
bool GameController::is2x2Passage(int startRow, int startCol) const
{
    if (!wall) return false;

    int rows = wall->getRows();
    int cols = wall->getCols();

    // 检查边界
    if (startRow < 0 || startRow + 1 >= rows ||
        startCol < 0 || startCol + 1 >= cols) {
        return false;
    }

    // 检查4个单元格是否都是通道（0）
    for (int r = 0; r < 2; ++r) {
        for (int c = 0; c < 2; ++c) {
            if (wall->getCellValue(startRow + r, startCol + c) != 0) {
                return false;
            }
        }
    }

    return true;
}

// 重新设计的物品生成算法 - 解决物品扎堆问题
void GameController::generateItemsSimple(int count)
{
    if (!wall || count <= 0) {
        return;
    }

    clearItems();
    qDebug() << "开始使用修复算法生成物品，目标数量:" << count;

    int rows = wall->getRows();
    int cols = wall->getCols();
    int cellSize = wall->getCellSize();

    qDebug() << "迷宫尺寸: " << rows << "x" << cols << "，单元格大小:" << cellSize;
    qDebug() << "寻找40x40通道区域(2x2的0区域)...";

    // 步骤1: 收集所有40x40通道区域的中心点
    QVector<QPointF> fortyFortyCenters;
    QVector<QPoint> fortyFortyGrids;  // 存储每个40x40区域的左上角网格坐标

    for (int row = 0; row < rows - 1; ++row) {  // 注意：row+1不能超出边界
        for (int col = 0; col < cols - 1; ++col) {  // 注意：col+1不能超出边界
            if (is2x2Passage(row, col)) {
                // 计算40x40区域的中心点
                // 左上角: (col*cellSize, row*cellSize)
                // 中心: (col*cellSize + 20, row*cellSize + 20)
                float centerX = col * cellSize + 20.0f;  // 20是40的一半
                float centerY = row * cellSize + 20.0f;

                if (centerX >= 15 && centerX <= maxX - 15 &&
                    centerY >= 15 && centerY <= maxY - 15) {
                    fortyFortyCenters.append(QPointF(centerX, centerY));
                    fortyFortyGrids.append(QPoint(col, row));
                }
            }
        }
    }

    qDebug() << "找到" << fortyFortyCenters.size() << "个40x40通道区域";

    if (fortyFortyCenters.isEmpty()) {
        qDebug() << "错误：没有找到任何40x40通道区域！尝试备用方案...";

        // 备用方案：在单个通道单元格中心生成
        QVector<QPointF> allCenters;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (wall->getCellValue(row, col) == 0) {
                    float centerX = col * cellSize + cellSize / 2.0f;
                    float centerY = row * cellSize + cellSize / 2.0f;

                    if (centerX >= 15 && centerX <= maxX - 15 &&
                        centerY >= 15 && centerY <= maxY - 15) {
                        allCenters.append(QPointF(centerX, centerY));
                    }
                }
            }
        }

        qDebug() << "备用方案：找到" << allCenters.size() << "个单个通道位置";
        fortyFortyCenters = allCenters;
    }

    if (fortyFortyCenters.isEmpty()) {
        qDebug() << "错误：没有找到任何通道位置！";
        return;
    }

    // 步骤2: 打乱位置列表
    for (int i = 0; i < fortyFortyCenters.size(); ++i) {
        int j = getRandomInt(0, fortyFortyCenters.size() - 1);
        qSwap(fortyFortyCenters[i], fortyFortyCenters[j]);
    }

    // 步骤3: 生成物品
    int generated = 0;

    // 记录每个已生成的40x40区域，避免在相邻区域生成
    QSet<QPoint> usedFortyFortyGrids;

    for (int i = 0; i < fortyFortyCenters.size() && generated < count; ++i) {
        QPointF center = fortyFortyCenters[i];

        // 检查是否与其他物品太近
        bool tooClose = false;
        float minDistance = 60.0f;  // 增加最小距离到60像素，避免扎堆

        for (Item* existingItem : items) {
            float dx = center.x() - existingItem->getPosition().x();
            float dy = center.y() - existingItem->getPosition().y();
            float distance = qSqrt(dx*dx + dy*dy);

            if (distance < minDistance) {
                tooClose = true;
                break;
            }
        }

        if (tooClose) {
            continue;
        }

        // 检查是否与墙重叠
        QRectF itemRect(center.x() - 15, center.y() - 15, 30, 30);
        QVector<QRectF> wallRects = wall->getWallRects();
        bool overlapsWithWall = false;

        for (const QRectF& wallRect : wallRects) {
            if (itemRect.intersects(wallRect)) {
                overlapsWithWall = true;
                break;
            }
        }

        if (overlapsWithWall) {
            continue;
        }

        // 创建物品
        int itemType = getRandomInt(0, 7);
        Item *newItem = new Item(this);
        newItem->setType(itemType);
        newItem->setPosition(center.x(), center.y());
        items.append(newItem);
        generated++;

        qDebug() << "生成物品" << generated << "类型" << itemType
                 << "在40x40通道中心(" << center.x() << "," << center.y() << ")";
    }

    qDebug() << "修复算法生成了" << generated << "个物品";

    if (itemView) {
        itemView->setItems(items);
    }
}

// 主物品生成函数
void GameController::generateItems(int count)
{
    qDebug() << "使用修复的物品生成算法...";
    generateItemsSimple(count);

    if (items.size() < count) {
        qDebug() << "第一次尝试只生成了" << items.size() << "个物品，尝试放宽条件...";

        int remaining = count - items.size();
        int attempts = 0;
        int maxAttempts = remaining * 100;

        // 收集所有通道位置
        int rows = wall->getRows();
        int cols = wall->getCols();
        int cellSize = wall->getCellSize();

        QVector<QPointF> allCenters;
        for (int row = 0; row < rows; ++row) {
            for (int col = 0; col < cols; ++col) {
                if (wall->getCellValue(row, col) == 0) {
                    float centerX = col * cellSize + cellSize / 2.0f;
                    float centerY = row * cellSize + cellSize / 2.0f;

                    if (centerX >= 15 && centerX <= maxX - 15 &&
                        centerY >= 15 && centerY <= maxY - 15) {
                        allCenters.append(QPointF(centerX, centerY));
                    }
                }
            }
        }

        // 打乱位置
        for (int i = 0; i < allCenters.size(); ++i) {
            int j = getRandomInt(0, allCenters.size() - 1);
            qSwap(allCenters[i], allCenters[j]);
        }

        while (items.size() < count && attempts < maxAttempts) {
            attempts++;

            if (allCenters.isEmpty()) break;

            // 从所有位置中随机选择
            int index = getRandomInt(0, allCenters.size() - 1);
            QPointF pos = allCenters[index];

            // 检查是否与其他物品太近
            bool tooClose = false;
            float minDistance = 30.0f;  // 降低距离要求

            for (Item* existingItem : items) {
                float dx = pos.x() - existingItem->getPosition().x();
                float dy = pos.y() - existingItem->getPosition().y();
                float distance = qSqrt(dx*dx + dy*dy);

                if (distance < minDistance) {
                    tooClose = true;
                    break;
                }
            }

            if (tooClose) {
                continue;
            }

            // 检查是否与墙重叠
            QRectF itemRect(pos.x() - 15, pos.y() - 15, 30, 30);
            QVector<QRectF> wallRects = wall->getWallRects();
            bool overlapsWithWall = false;

            for (const QRectF& wallRect : wallRects) {
                if (itemRect.intersects(wallRect)) {
                    overlapsWithWall = true;
                    break;
                }
            }

            if (overlapsWithWall) {
                continue;
            }

            // 创建物品
            int itemType = getRandomInt(0, 7);
            Item *newItem = new Item(this);
            newItem->setType(itemType);
            newItem->setPosition(pos.x(), pos.y());
            items.append(newItem);

            qDebug() << "放宽条件生成物品" << items.size() << "在位置(" << pos.x() << "," << pos.y() << ")";
        }
    }

    qDebug() << "物品生成完成，共生成" << items.size() << "个物品";

    if (items.size() > 0) {
        QMap<int, int> typeCount;
        int totalScorePotential = 0;

        for (Item* item : items) {
            int type = item->getType();
            typeCount[type] = typeCount.value(type, 0) + 1;
            totalScorePotential += item->getScore();
        }

        qDebug() << "物品类型分布:";
        for (int i = 0; i < 8; ++i) {
            int count = typeCount.value(i, 0);
            if (count > 0) {
                qDebug() << "  类型" << i << "(" << count << "个) 分数:" << Item::getScoreByType(i);
            }
        }
        qDebug() << "所有物品总分数:" << totalScorePotential << "分";

        // 计算物品间平均距离
        if (items.size() > 1) {
            float totalDistance = 0;
            int pairCount = 0;

            for (int i = 0; i < items.size(); ++i) {
                for (int j = i + 1; j < items.size(); ++j) {
                    float dx = items[i]->getPosition().x() - items[j]->getPosition().x();
                    float dy = items[i]->getPosition().y() - items[j]->getPosition().y();
                    float distance = qSqrt(dx*dx + dy*dy);
                    totalDistance += distance;
                    pairCount++;
                }
            }

            if (pairCount > 0) {
                float avgDistance = totalDistance / pairCount;
                qDebug() << "物品间平均距离:" << avgDistance << "像素";
            }
        }
    }

    if (items.size() < 8) {
        qDebug() << "警告: 最终只生成了" << items.size() << "个物品，未达到8个目标";
    } else {
        qDebug() << "成功生成8个物品！";
    }
}
