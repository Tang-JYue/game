#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QPointF>
#include <QRectF>

class Player : public QObject
{
    Q_OBJECT

public:
    explicit Player(QObject *parent = nullptr);

    // 基本属性
    QPointF getPosition() const;
    void setPosition(float x, float y);
    void setPosition(const QPointF& pos);

    // 移动控制
    void moveUp();
    void moveDown();
    void moveLeft();
    void moveRight();
    void stop();

    // 更新位置
    void updatePosition();

    // 状态查询
    bool isMoving() const;

    // 速度控制
    float getSpeed() const;
    void setSpeed(float speed);

    // 碰撞框
    QRectF getBoundingBox() const;
    float getWidth() const;
    float getHeight() const;

    // 血量系统
    int getHealth() const;
    int getMaxHealth() const;
    void setHealth(int health);
    void setMaxHealth(int maxHealth);
    void takeDamage(int damage);
    void heal(int amount);
    bool isDead() const;

signals:
    void healthChanged(int health, int maxHealth);
    void playerDied();
    void positionChanged();

private:
    // 位置和移动
    QPointF position;
    float speed = 3.0f;
    float moveX = 0;
    float moveY = 0;
    bool moving = false;

    // 尺寸 - 18x30
    float width = 18.0f;
    float height = 30.0f;

    // 血量
    int health = 5;
    int maxHealth = 5;
};

#endif // PLAYER_H
