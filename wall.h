#ifndef WALL_H
#define WALL_H

#include <QPoint>
#include <QRectF>
#include <QVector>

class Wall
{
public:
    Wall();

    // 定义迷宫尺寸
    static const int MAZE_ROWS = 61;
    static const int MAZE_COLS = 61;
    static const int CELL_SIZE = 20;

    // 碰撞检测
    bool checkCollision(const QRectF& rect) const;
    bool isPointInWall(float x, float y) const;
    int getCellValue(int row, int col) const;

    // 获取迷宫数据
    const int (*getMazeData() const)[MAZE_COLS];

    // 获取迷宫尺寸
    int getRows() const;
    int getCols() const;
    int getCellSize() const;

    // 获取伤害值
    int getDamage() const;
    void setDamage(int damage);

    // 获取墙壁矩形列表
    QVector<QRectF> getWallRects() const;

private:
    int mazeData[MAZE_ROWS][MAZE_COLS];
    int wallDamage = 1;

    void initializeMazeData();
    QPoint worldToGrid(float worldX, float worldY) const;
};

#endif // WALL_H
