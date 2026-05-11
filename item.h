#ifndef ITEM_H
#define ITEM_H

#include <QObject>
#include <QPointF>
#include <QRectF>
#include <QString>

class Item : public QObject
{
    Q_OBJECT
public:
    explicit Item(QObject *parent = nullptr);

    // 位置
    void setPosition(float x, float y);
    QPointF getPosition() const;

    // 类型和分数
    int getType() const;
    int getScore() const;
    QString getImagePath() const;
    void setType(int type);

    // ✅ 新增：通过类型获取分数的公共静态方法
    static int getScoreByType(int type);

    // 碰撞框 - 改为30x30
    QRectF getBoundingBox() const;

    // 状态
    bool isCollected() const;
    void setCollected(bool collected);

    // 是否显示提示
    bool isShowPrompt() const;
    void setShowPrompt(bool show);

    // 重置
    void reset();

signals:
    void collectedChanged(bool collected);
    void positionChanged();

private:
    QPointF position;
    int type = 0;  // 0-7
    int score = 0;
    bool collected = false;
    bool showPrompt = false;

    // 物品配置
    static const int ITEM_SCORES[8];
    static const QString ITEM_PATHS[8];
};

#endif // ITEM_H
