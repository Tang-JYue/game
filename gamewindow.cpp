#include "gamewindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QCloseEvent>

#include "player.h"
#include "playerview.h"
#include "wall.h"
#include "wallview.h"
#include "healthview.h"
#include "item.h"
#include "itemview.h"
#include "door.h"
#include "gamecontroller.h"

GameWindow::GameWindow(QWidget *parent) : QWidget(parent)
{
    qDebug() << "===== 游戏初始化开始 =====";

    // 创建游戏对象
    wall = new Wall();
    player = new Player();
    door = new Door();

    // 计算窗口大小
    int mazeWidth = wall->getCols() * wall->getCellSize();
    int mazeHeight = wall->getRows() * wall->getCellSize();
    int healthBarHeight = 60;

    // 设置窗口
    setWindowTitle("故事收集游戏");
    setFixedSize(mazeWidth, mazeHeight + healthBarHeight);
    setFocusPolicy(Qt::StrongFocus);

    setupUI();

    qDebug() << "迷宫尺寸:" << mazeWidth << "x" << mazeHeight;
    qDebug() << "窗口尺寸:" << width() << "x" << height();
}

GameWindow::~GameWindow()
{
    delete gameController;
    delete player;
    delete wall;
    delete door;
    qDebug() << "游戏窗口销毁";
}

void GameWindow::setupUI()
{
    // 获取迷宫尺寸
    int mazeWidth = wall->getCols() * wall->getCellSize();
    int mazeHeight = wall->getRows() * wall->getCellSize();
    int healthBarHeight = 60;

    // 1. 创建血量视图
    healthView = new HealthView(this);
    healthView->setGeometry(0, 0, mazeWidth, healthBarHeight);
    qDebug() << "血量视图创建完成";

    // 2. 创建墙视图
    wallView = new WallView(this);
    wallView->setGeometry(0, healthBarHeight, mazeWidth, mazeHeight);
    wallView->setWall(wall);
    qDebug() << "墙视图创建完成";

    // 3. 创建物品视图
    itemView = new ItemView(this);
    itemView->setGeometry(0, healthBarHeight, mazeWidth, mazeHeight);
    qDebug() << "物品视图创建完成";

    // 4. 设置玩家初始位置
    float startX = 5 * wall->getCellSize() + wall->getCellSize()/2;
    float startY = 3 * wall->getCellSize() + wall->getCellSize()/2;
    player->setPosition(startX, startY);

    qDebug() << "玩家创建完成，初始位置: ("
             << player->getPosition().x() << ", "
             << player->getPosition().y() << ")";
    qDebug() << "玩家尺寸:" << player->getWidth() << "x" << player->getHeight();

    // 5. 创建玩家视图
    playerView = new PlayerView(this);
    playerView->setGeometry(0, healthBarHeight, mazeWidth, mazeHeight);
    playerView->setPlayer(player);

    QString playerImagePath = ":/new/prefix1/player.png";
    playerView->setImage(playerImagePath);

    qDebug() << "玩家视图创建完成";

    // 6. 设置血量视图
    healthView->setPlayer(player);
    qDebug() << "血量视图关联玩家";

    // 7. 创建游戏控制器
    gameController = new GameController(this);
    gameController->initialize(player, playerView, wall, healthView, itemView, door);
    healthView->setGameController(gameController);

    // 8. 连接信号
    connect(gameController, &GameController::collisionDetected,
            this, &GameWindow::onCollision);
    connect(gameController, &GameController::itemCollected,
            this, &GameWindow::onItemCollected);
    connect(gameController, &GameController::doorOpened,
            this, &GameWindow::onDoorOpened);
    connect(gameController, &GameController::gameWon,
            this, &GameWindow::onGameWon);
    connect(gameController, &GameController::showItemPrompt,
            this, &GameWindow::onShowItemPrompt);
    connect(gameController, &GameController::showStoryDialog,
            this, &GameWindow::onShowStoryDialog);
    connect(player, &Player::playerDied,
            this, &GameWindow::onPlayerDied);
    connect(player, &Player::positionChanged,
            playerView, QOverload<>::of(&QWidget::update));
    connect(player, &Player::healthChanged,
            healthView, &HealthView::updateDisplay);

    // 启动游戏
    gameController->startGame();

    qDebug() << "===== 游戏初始化完成 =====";
    qDebug() << "玩家初始血量:" << player->getHealth() << "/" << player->getMaxHealth();
    qDebug() << "控制说明:";
    qDebug() << "  W/A/S/D  - 上下左右移动";
    qDebug() << "  C        - 收集物品";
    qDebug() << "  R        - 阅读故事";
    qDebug() << "  空格     - 暂停/继续游戏";
    qDebug() << "  ESC      - 退出游戏";
    qDebug() << "注意：撞墙会减少1点血量！";
    qDebug() << "目标：收集物品达到560分打开门";
    qDebug() << "==========================";
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{
    qDebug() << "按键按下:" << event->key();
    if (gameController) {
        gameController->handleKeyPress(event);
    }
}

void GameWindow::keyReleaseEvent(QKeyEvent* event)
{
    if (gameController) {
        gameController->handleKeyRelease(event);
    }
}

void GameWindow::closeEvent(QCloseEvent* event)
{
    Q_UNUSED(event);
    if (gameController) {
        gameController->stopGame();
    }
    QWidget::closeEvent(event);
}

void GameWindow::onCollision()
{
    if (player) {
        qDebug() << "发生碰撞！当前血量:" << player->getHealth();
    }
}

void GameWindow::onPlayerDied()
{
    qDebug() << "玩家死亡！游戏结束";

    if (gameController) {
        gameController->stopGame();
    }

    QMessageBox::critical(this, "游戏结束",
        "你被墙壁撞死了！\n"
        "最终分数: " + QString::number(gameController->getTotalScore()) + "\n"
        "收集物品: " + QString::number(gameController->getItemsCollected()) + "个");

    onRestartGame();
}

void GameWindow::onItemCollected(int score, int total)
{
    qDebug() << "收集物品 +" << score << "分，总分:" << total;

    // 更新血量视图
    if (healthView) {
        healthView->updateDisplay();
    }

    // 如果达到目标分数，显示提示
    if (total >= 560) {
        QMessageBox::information(this, "恭喜！",
            "你已经收集了560分！\n"
            "门已打开，现在可以进入门内获得胜利！");
    }
}

void GameWindow::onDoorOpened()
{
    qDebug() << "门已打开！";

    QMessageBox::information(this, "门已打开",
        "门已经打开了！\n"
        "前往迷宫右下角进入门内获得胜利！");
}

void GameWindow::onGameWon()
{
    QMessageBox::information(this, "游戏胜利！",
        "恭喜你获胜！\n"
        "最终分数: " + QString::number(gameController->getTotalScore()) + "\n"
        "收集物品: " + QString::number(gameController->getItemsCollected()) + "个");

    onRestartGame();
}

void GameWindow::onShowItemPrompt(Item* item)
{
    if (item) {
        qDebug() << "接触到物品，按C收集，按R阅读故事";

        // 在控制台显示提示
        qDebug() << "=== 物品提示 ===";
        qDebug() << "类型:" << item->getType() + 1;
        qDebug() << "分数:" << item->getScore();
        qDebug() << "按C收集，按R阅读故事";
        qDebug() << "=============";
    }
}

void GameWindow::onShowStoryDialog(const QString& story)
{
    QMessageBox::information(this, "故事碎片", story);
}

void GameWindow::onRestartGame()
{
    qDebug() << "重新开始游戏...";

    // 重置游戏
    if (gameController) {
        delete gameController;
    }

    if (player) {
        delete player;
    }

    if (door) {
        delete door;
    }

    // 重新创建对象
    player = new Player();
    door = new Door();

    // 重新设置玩家位置
    float startX = 5 * wall->getCellSize() + wall->getCellSize()/2;
    float startY = 3 * wall->getCellSize() + wall->getCellSize()/2;
    player->setPosition(startX, startY);

    // 重新设置视图
    playerView->setPlayer(player);
    healthView->setPlayer(player);

    // 重新创建控制器
    gameController = new GameController(this);
    gameController->initialize(player, playerView, wall, healthView, itemView, door);
    healthView->setGameController(gameController);

    // 重新连接信号
    connect(gameController, &GameController::collisionDetected,
            this, &GameWindow::onCollision);
    connect(gameController, &GameController::itemCollected,
            this, &GameWindow::onItemCollected);
    connect(gameController, &GameController::doorOpened,
            this, &GameWindow::onDoorOpened);
    connect(gameController, &GameController::gameWon,
            this, &GameWindow::onGameWon);
    connect(gameController, &GameController::showItemPrompt,
            this, &GameWindow::onShowItemPrompt);
    connect(gameController, &GameController::showStoryDialog,
            this, &GameWindow::onShowStoryDialog);
    connect(player, &Player::playerDied,
            this, &GameWindow::onPlayerDied);
    connect(player, &Player::positionChanged,
            playerView, QOverload<>::of(&QWidget::update));
    connect(player, &Player::healthChanged,
            healthView, &HealthView::updateDisplay);

    // 重新开始游戏
    gameController->startGame();
}

void GameWindow::onQuitGame()
{
    close();
}
