#include "door.h"
#include <QDebug>

Door::Door(QObject *parent) : QObject(parent)
{
    // 默认位置在右下角
    position = QPointF(1150.0f, 1150.0f);
}

void Door::setPosition(float x, float y)
{
    position.setX(x);
    position.setY(y);
}

QPointF Door::getPosition() const
{
    return position;
}

bool Door::isOpen() const
{
    return open;
}

void Door::setOpen(bool isOpen)
{
    if (open != isOpen) {
        open = isOpen;
        qDebug() << "门状态:" << (open ? "已打开" : "已关闭");
        emit doorStateChanged(open);
    }
}

QRectF Door::getBoundingBox() const
{
    return QRectF(position.x() - 20, position.y() - 20, 40, 40);
}

void Door::reset()
{
    open = false;
}
