#ifndef DOOR_H
#define DOOR_H

#include <QObject>
#include <QPointF>
#include <QRectF>

class Door : public QObject
{
    Q_OBJECT

public:
    explicit Door(QObject *parent = nullptr);

    // 位置
    void setPosition(float x, float y);
    QPointF getPosition() const;

    // 状态
    bool isOpen() const;
    void setOpen(bool open);

    // 碰撞框
    QRectF getBoundingBox() const;

    // 重置
    void reset();

signals:
    void doorStateChanged(bool open);

private:
    QPointF position;
    bool open = false;
};

#endif // DOOR_H

