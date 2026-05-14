#include "item.h"

// 物品分数配置
const int Item::ITEM_SCORES[8] = {12, 18, 25, 35, 48, 64, 85, 110};

// 物品图片路径
const QString Item::ITEM_PATHS[8] = {
    ":/new/prefix1/item01.png",
    ":/new/prefix1/item02.png",
    ":/new/prefix1/item03.png",
    ":/new/prefix1/item04.png",
    ":/new/prefix1/item05.png",
    ":/new/prefix1/item06.png",
    ":/new/prefix1/item07.png",
    ":/new/prefix1/item08.png",
};

Item::Item(QObject *parent) : QObject(parent)
{
    // 默认生成在(0,0)，之后会被重新设置
    position = QPointF(0, 0);
}

void Item::setPosition(float x, float y)
{
    if (position.x() != x || position.y() != y) {
        position.setX(x);
        position.setY(y);
        emit positionChanged();
    }
}

QPointF Item::getPosition() const
{
    return position;
}

int Item::getType() const
{
    return type;
}

int Item::getScore() const
{
    return score;
}

QString Item::getImagePath() const
{
    if (type >= 0 && type < 8) {
        return ITEM_PATHS[type];
    }
    return "";
}

void Item::setType(int t)
{
    if (t >= 0 && t < 8) {
        type = t;
        score = ITEM_SCORES[t];
    }
}

// ✅ 新增：实现公共静态方法
int Item::getScoreByType(int type)
{
    if (type >= 0 && type < 8) {
        return ITEM_SCORES[type];
    }
    return 0;
}

QRectF Item::getBoundingBox() const
{
    // 改为30x30大小
    int size = 30;
    return QRectF(position.x() - size/2, position.y() - size/2, size, size);
}

bool Item::isCollected() const
{
    return collected;
}

void Item::setCollected(bool c)
{
    if (collected != c) {
        collected = c;
        emit collectedChanged(collected);
    }
}

bool Item::isShowPrompt() const
{
    return showPrompt;
}

void Item::setShowPrompt(bool show)
{
    if (showPrompt != show) {
        showPrompt = show;
    }
}

void Item::reset()
{
    collected = false;
    showPrompt = false;
}
