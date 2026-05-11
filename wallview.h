#ifndef WALLVIEW_H
#define WALLVIEW_H

#include <QWidget>
#include <QPixmap>

class Wall;

class WallView : public QWidget
{
    Q_OBJECT

public:
    explicit WallView(QWidget *parent = nullptr);

    void setWall(Wall* wall);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Wall* wall = nullptr;
    QPixmap wallPixmap;

    void createDefaultWallTexture();
    bool loadWallTexture(const QString& imagePath);
};

#endif // WALLVIEW_H
