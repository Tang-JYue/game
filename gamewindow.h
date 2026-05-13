#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include <QWidget>
#include <QKeyEvent>
#include <QCloseEvent>
#include <QMainWindow>
#include <QPainter>

// 前向声明
class Player;
class PlayerView;
class Wall;
class WallView;
class HealthView;
class Item;
class ItemView;
class GameController;

class GameWindow : public QWidget
{
    Q_OBJECT

public:
    GameWindow(QWidget *parent = nullptr);
    ~GameWindow();

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void closeEvent(QCloseEvent* event) override;

private slots:
    void onCollision();
    void onPlayerDied();
    void onItemCollected(int score, int total);
    void onGameWon();
    void onShowItemPrompt(Item* item);
    void onShowStoryFragmentView(const QString& story, const QString& imagePath);
    void onRestartGame();
    void onQuitGame();
    //+++++++++++++++++++++++++
    void onFinalStoryTriggered();
    //++++++++++++++++++++++++++++++

private:
    void setupUI();

    // 游戏对象
    Player* player = nullptr;
    PlayerView* playerView = nullptr;
    Wall* wall = nullptr;
    WallView* wallView = nullptr;
    HealthView* healthView = nullptr;
    ItemView* itemView = nullptr;
    GameController* gameController = nullptr;
};

#endif // GAMEWINDOW_H
