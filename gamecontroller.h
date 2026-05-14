#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <QObject>
#include <QTimer>
#include <QVector>
#include <QPointF>

class Player;
class PlayerView;
class Wall;
class HealthView;
class Item;
class ItemView;
class StoryFragment;
class QKeyEvent;

class GameController : public QObject
{
    Q_OBJECT

public:
    explicit GameController(QObject *parent = nullptr);
    ~GameController();

    enum GameState { Stopped, Running, Paused };

    // 初始化
    void initialize(Player* player, PlayerView* playerView, Wall* wall,
                    HealthView* healthView, ItemView* itemView);

    // 游戏控制
    void startGame();
    void stopGame();
    void pauseGame();
    void resumeGame();

    // 输入处理
    void handleKeyPress(QKeyEvent* event);
    void handleKeyRelease(QKeyEvent* event);

    // 获取状态
    GameState getGameState() const;
    int getTotalScore() const;
    int getItemsCollected() const;

    // 获取当前物品
    Item* getCurrentItem() const;
    QString getCurrentStory() const;

    // 清空物品
    void clearItems();

    // 收集当前物品
    void collectCurrentItem();

    // 显示故事
    void showStoryFragment(int itemType);

    // 随机数生成
    int getRandomInt(int min, int max);

signals:
    void gameStarted();
    void gameStopped();
    void gamePaused();
    void gameResumed();
    void gameWon();

    void itemCollected(int score, int totalScore);
    void showStoryFragmentView(const QString& story, const QString& imagePath);
    void showItemPrompt(Item* item);
    void collisionDetected();
    void finalStoryTriggered();
    void playerDied(quint64 instanceId);

private slots:
    void updateGame();

private:
    // 游戏状态
    GameState gameState = Stopped;

    // 游戏对象
    Player* player = nullptr;
    PlayerView* playerView = nullptr;
    Wall* wall = nullptr;
    HealthView* healthView = nullptr;
    ItemView* itemView = nullptr;

    // 游戏逻辑
    QTimer* gameTimer = nullptr;
    int frameRate = 60;

    // 物品管理
    QVector<Item*> items;
    Item* currentItem = nullptr;

    // 分数和血量
    int totalScore = 0;
    int itemsCollected = 0;
    QString currentStory;

    // 故事碎片
    QVector<StoryFragment*> storyFragments;

    // 碰撞检测
    int collisionCooldown = 0;
    static constexpr int COLLISION_COOLDOWN_FRAMES = 10;

    // 开门分数阈值
    static constexpr int DOOR_SCORE_THRESHOLD = 612;

    // 边界
    float minX = 0;
    float maxX = 800;
    float minY = 0;
    float maxY = 600;

    // 初始化故事
    void initializeStories();

    // 游戏逻辑处理
    void processPlayerMovement();
    void checkBoundaries();
    void processWallCollision();
    void processItemCollision();
    void readCurrentItemStory();

    // 位置生成
    QPointF getRandomValidPosition();
    bool isValidPosition(const QPointF& pos) const;

    // 3x3通道检查
    bool is3x3Passage(int centerRow, int centerCol) const;

    // 2x2通道检查
    bool is2x2Passage(int startRow, int startCol) const;

    // 物品生成
    void generateItems(int count);
    void generateItemsSimple(int count);
};

#endif // GAMECONTROLLER_H
