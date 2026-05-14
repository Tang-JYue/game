#include "wallview.h"
#include "wall.h"
#include <QPainter>
#include <QDebug>

WallView::WallView(QWidget *parent) : QWidget(parent)
{
    // 先尝试加载图片，如果失败则使用默认纹理
    QString wallImagePath = ":/new/prefix1/wall1.png";
    if (!loadWallTexture(wallImagePath)) {
        createDefaultWallTexture();
    }
}

void WallView::setWall(Wall* w)
{
    wall = w;
    if (wall) {
        int width = wall->getCols() * wall->getCellSize();
        int height = wall->getRows() * wall->getCellSize();
        setFixedSize(width, height);
    }
    update();
}

void WallView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (!wall) return;

    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    int rows = wall->getRows();
    int cols = wall->getCols();
    int cellSize = wall->getCellSize();
    const int (*mazeData)[61] = wall->getMazeData();

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            if (mazeData[row][col] == 1) {
                int x = col * cellSize;
                int y = row * cellSize;
                painter.drawPixmap(x, y, cellSize, cellSize, wallPixmap);
            }
        }
    }
}

bool WallView::loadWallTexture(const QString& imagePath)
{
    QPixmap tempPixmap;
    if (tempPixmap.load(imagePath)) {
        // 缩放图片到20x20
        wallPixmap = tempPixmap.scaled(20, 20, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        return true;
    } else {
        qDebug() << "墙壁图片加载失败:" << imagePath;
        return false;
    }
}

void WallView::createDefaultWallTexture()
{
    wallPixmap = QPixmap(20, 20);
    wallPixmap.fill(QColor(150, 100, 50));

    QPainter painter(&wallPixmap);
    painter.setPen(QPen(QColor(100, 70, 30), 2));

    // 简单砖块纹理
    painter.drawRect(0, 0, 20, 20);
    painter.drawLine(10, 0, 10, 20);
    painter.drawLine(0, 10, 20, 10);
}
