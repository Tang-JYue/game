#include "gamewindow.h"
#include "finalstoryview.h"
#include <QApplication>
#include <QMessageBox>
#include <QDebug>
#include <QCloseEvent>
#include <QPainter>
#include <QPen>
#include <QCoreApplication>

#include "player.h"
#include "playerview.h"
#include "wall.h"
#include "wallview.h"
#include "healthview.h"
#include "item.h"
#include "itemview.h"
#include "gamecontroller.h"
#include "storyfragmentview.h"
#include "failview.h"


GameWindow::GameWindow(QWidget *parent) : QWidget(parent)
{
    // 创建游戏对象
    wall = new Wall();
    player = new Player();

    // 计算窗口大小
    int mazeWidth = wall->getCols() * wall->getCellSize();
    int mazeHeight = wall->getRows() * wall->getCellSize();
    int healthBarHeight = 60;

    // 设置窗口
    setWindowTitle("故事收集游戏");
    setFixedSize(mazeWidth, mazeHeight + healthBarHeight);
    setFocusPolicy(Qt::StrongFocus);

    setupUI();
}

GameWindow::~GameWindow()
{
    delete gameController;
    delete player;
    delete wall;
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

    // 2. 创建墙视图
    wallView = new WallView(this);
    wallView->setGeometry(0, healthBarHeight, mazeWidth, mazeHeight);
    wallView->setWall(wall);

    // 3. 创建物品视图
    itemView = new ItemView(this);
    itemView->setGeometry(0, healthBarHeight, mazeWidth, mazeHeight);

    // 4. 设置玩家初始位置
    QPointF safeSpawn = findSafePlayerSpawn();
    player->setPosition(safeSpawn);
    qDebug() << "设置玩家安全出生点: (" << safeSpawn.x() << ", " << safeSpawn.y() << ")";
    m_currentValidPlayerId = player->getInstanceId();
    qDebug() << "[GameWindow] 初始玩家ID设置为: " << m_currentValidPlayerId;

    // 5. 创建玩家视图
    playerView = new PlayerView(this);
    playerView->setGeometry(0, healthBarHeight, mazeWidth, mazeHeight);
    playerView->setPlayer(player);

    QString playerImagePath = ":/new/prefix1/player.png";
    playerView->setImage(playerImagePath);

    // 6. 设置血量视图
    healthView->setPlayer(player);

    // 7. 创建游戏控制器
    gameController = new GameController(this);
    gameController->initialize(player, playerView, wall, healthView, itemView);

    if (gameController->getTotalScore() > 0) {
        qWarning() << "错误：游戏开始时分数不为0！";
        // 强制重置分数
    }
    healthView->setGameController(gameController);

    // 8. 连接信号
    connect(gameController, &GameController::itemCollected,
            this, &GameWindow::onItemCollected);
    connect(gameController, &GameController::finalStoryTriggered,
            this, &GameWindow::onFinalStoryTriggered);
    connect(gameController, &GameController::gameWon,
            this, &GameWindow::onGameWon);
    connect(gameController, &GameController::showStoryFragmentView,
            this, &GameWindow::onShowStoryFragmentView);
    connect(gameController, &GameController::playerDied,
            this, &GameWindow::onPlayerDied);
    connect(player, &Player::positionChanged,
            playerView, QOverload<>::of(&QWidget::update));
    connect(player, &Player::healthChanged,
            healthView, &HealthView::updateDisplay);


    // 启动游戏
    gameController->startGame();
}

void GameWindow::keyPressEvent(QKeyEvent* event)
{
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


void GameWindow::onPlayerDied(quint64 diedPlayerId)
{
    qDebug() << "[onPlayerDied] 收到信号，来自玩家ID:" << diedPlayerId
                 << "，当前有效ID:" << m_currentValidPlayerId;

        // 1. 如果当前有效ID为0（未初始化或已重置），检查是否是合理的玩家ID
        if (m_currentValidPlayerId == 0) {
            if (player && gameController &&
                gameController->getGameState() == GameController::Running) {
                qDebug() << "  -> 接受此信号（可能是初始化问题）";
                // 继续执行死亡处理
            } else {
                qDebug() << "  -> 忽略！游戏未运行或玩家不存在";
                return;
            }
        }
        // 2. 正常情况下的ID验证
        else if (diedPlayerId != m_currentValidPlayerId) {
            qDebug() << "  -> 忽略！这不是当前有效玩家的死亡信号。";
            return;
        }

        // 死亡处理逻辑
        qDebug() << "玩家死亡！游戏结束";

        if (gameController) {
            gameController->stopGame();
        }

        // 显示失败窗口
        FailView *failView = new FailView(this);
        failView->setAttribute(Qt::WA_DeleteOnClose);
        connect(failView, &QDialog::accepted, this, &GameWindow::onRestartGame);
        connect(failView, &QDialog::rejected, this, &GameWindow::onQuitGame);
        failView->exec();
}

void GameWindow::onItemCollected(int score, int total)
{
    Q_UNUSED(total);
    // 更新血量视图
    if (healthView) {
        healthView->updateDisplay();
    }
}

void GameWindow::onGameWon()
{
     qDebug() << "游戏胜利，分数已达到最终目标！";
}

void GameWindow::onShowStoryFragmentView(const QString& story, const QString& imagePath)
{
    // 游戏暂停，确保弹出对话框时游戏逻辑暂停
    if (gameController && gameController->getGameState() == GameController::Running) {
        gameController->pauseGame();
    }

    // 创建并显示新的故事碎片视图 (StoryFragmentView)
    StoryFragmentView *view = new StoryFragmentView(story, imagePath, this);
    view->setAttribute(Qt::WA_DeleteOnClose); // 对话框关闭时自动删除
    view->exec(); // 使用exec()模态显示，阻塞直到关闭

    // 对话框关闭后，恢复游戏
    if (gameController) {
        // 无论当前是什么状态，都尝试恢复游戏
        // 因为对话框显示期间，游戏应该处于暂停状态
        if (gameController->getGameState() == GameController::Paused) {
            gameController->resumeGame();
        } else {
            // 如果状态不是Paused，可能是某些原因导致状态异常
            // 强制恢复游戏运行状态
            gameController->startGame();
        }

        // 确保游戏窗口重新获得焦点
        // 在函数末尾，恢复游戏后添加：
        this->setFocus();
        this->activateWindow();
    }
}

void GameWindow::onFinalStoryTriggered()
{
    // 游戏暂停
    if (gameController && gameController->getGameState() == GameController::Running) {
        gameController->pauseGame();
    }

    // 使用新的 FinalStoryView 显示最终故事
    FinalStoryView *view = new FinalStoryView(this);
    view->setAttribute(Qt::WA_DeleteOnClose);

    // 连接对话框按钮信号
    connect(view, &FinalStoryView::accepted, this, &GameWindow::onRestartGame);
    connect(view, &FinalStoryView::rejected, this, &GameWindow::onQuitGame);

    view->exec();
}

void GameWindow::onRestartGame()
{
    qDebug() << "=== 开始重启游戏 ===";

       // 将当前有效玩家ID标记为无效
       m_currentValidPlayerId = 0;
       qDebug() << "[重启] 重置当前有效玩家ID为0（无效）。";

       // 1. 停止游戏
       if (gameController) {
           gameController->stopGame();
       }

    // 2.彻底断开所有旧信号并清除事件队列
    // 2.1 先断开旧控制器与本窗口的所有连接
    if (gameController) {
        // 断开信号连接
        gameController->disconnect(this);

        //清除已发布但未处理的事件
        QCoreApplication::removePostedEvents(gameController);
    }

    // 2.2 断开旧玩家与本窗口的所有连接
    if (player) {
        // 断开信号连接
        player->disconnect(this);

        // 清除已发布但未处理的事件
        QCoreApplication::removePostedEvents(player);

        // 断开与视图的连接
        if (playerView) {
            player->disconnect(playerView);
        }
        if (healthView) {
            player->disconnect(healthView);
        }
    }

    // 3.处理当前窗口的事件队列
    // 清除所有已发布到本窗口但未处理的Player::playerDied事件
    QCoreApplication::removePostedEvents(this, QEvent::MetaCall);

    // 4. 重置视图指针
    if (playerView) {
        playerView->setPlayer(nullptr);
    }
    if (healthView) {
        healthView->setPlayer(nullptr);
        healthView->setGameController(nullptr);
    }
    if (itemView) {
        itemView->setItems(QVector<Item*>());
    }

    // 5. 保存旧指针用于删除
    GameController* oldController = gameController;
    Player* oldPlayer = player;

    // 6. 立即将指针置为nullptr
    gameController = nullptr;
    player = nullptr;

    // 7. 删除旧对象
    delete oldController;
    delete oldPlayer;

    // 8. 强制处理所有待处理事件
    QCoreApplication::processEvents();

    // 9. 重新创建玩家
    player = new Player();
    //记录新玩家的ID为当前有效ID
    m_currentValidPlayerId = player->getInstanceId();
    qDebug() << "[重启] 设置当前有效玩家ID为: " << m_currentValidPlayerId;

    // 重新设置玩家位置
    QPointF safeSpawn = findSafePlayerSpawn();
    player->setPosition(safeSpawn);
    qDebug() << "[重启] 设置新玩家安全出生点: (" << safeSpawn.x() << ", " << safeSpawn.y() << ")";

    // 10. 重新设置视图
    playerView->setPlayer(player);
    healthView->setPlayer(player);

    // 11. 重新创建控制器
    gameController = new GameController(this);
    gameController->initialize(player, playerView, wall, healthView, itemView);
    healthView->setGameController(gameController);

    // 12. 重新连接信号
    connect(gameController, &GameController::itemCollected,
            this, &GameWindow::onItemCollected);
    connect(gameController, &GameController::gameWon,
            this, &GameWindow::onGameWon);
    connect(gameController, &GameController::showStoryFragmentView,
            this, &GameWindow::onShowStoryFragmentView);
    connect(gameController, &GameController::finalStoryTriggered,
            this, &GameWindow::onFinalStoryTriggered);
    connect(gameController, &GameController::playerDied, // 信号源改为gameController
            this, &GameWindow::onPlayerDied);
    connect(player, &Player::positionChanged,
            playerView, QOverload<>::of(&QWidget::update));
    connect(player, &Player::healthChanged,
            healthView, &HealthView::updateDisplay);

    // 13. 重新开始游戏
    qDebug() << "开始新游戏...";
    gameController->startGame();

    // 14. 确保焦点
    this->setFocus();
    this->activateWindow();

    qDebug() << "=== 重启游戏完成 ===";
}

void GameWindow::onQuitGame()
{
    QApplication::quit();  // 退出整个应用程序
}

QPointF GameWindow::findSafePlayerSpawn() {
    if (!wall) {
        return QPointF(100, 100); // 备用位置
    }

    int cellSize = wall->getCellSize();
    int rows = wall->getRows();
    int cols = wall->getCols();

    // 玩家的碰撞框在网格中的“半径”（向上取整）
    int playerGridWidth = ceil(18.0f / cellSize);  // 约1个格子宽
    int playerGridHeight = ceil(30.0f / cellSize); // 约2个格子高

    // 我们需要找一个区域，其宽度>=playerGridWidth，高度>=playerGridHeight，且全是通道(0)
    for (int startRow = 0; startRow < rows - playerGridHeight; ++startRow) {
        for (int startCol = 0; startCol < cols - playerGridWidth; ++startCol) {

            // 检查这个矩形区域是否全是通道
            bool areaIsClear = true;
            for (int r = 0; r < playerGridHeight && areaIsClear; ++r) {
                for (int c = 0; c < playerGridWidth && areaIsClear; ++c) {
                    if (wall->getCellValue(startRow + r, startCol + c) != 0) {
                        areaIsClear = false;
                    }
                }
            }

            if (areaIsClear) {
                // 找到安全区域！返回这个区域的中心点（世界坐标）
                float centerX = (startCol + playerGridWidth / 2.0f) * cellSize;
                float centerY = (startRow + playerGridHeight / 2.0f) * cellSize;
                qDebug() << "[安全出生点] 找到区域: 网格(" << startCol << "," << startRow
                         << "), 世界坐标(" << centerX << "," << centerY << ")";
                return QPointF(centerX, centerY);
            }
        }
    }

    // 如果没找到（理论上不会，因为迷宫有大量通道），返回一个已知的备用点
    // 这里我们可以选一个更保守的位置，比如第一个通道格子的中心
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            if (wall->getCellValue(r, c) == 0) {
                float centerX = c * cellSize + cellSize / 2.0f;
                float centerY = r * cellSize + cellSize / 2.0f;
                return QPointF(centerX, centerY);
            }
        }
    }

    return QPointF(100, 100); // 最终备用
}
