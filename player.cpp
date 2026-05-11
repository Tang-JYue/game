#include "player.h"
#include <QDebug>

Player::Player(QObject *parent) : QObject(parent)
{
    // 初始位置 - 在迷宫中找到一个安全的起始位置
    // 检查迷宫数据，找到第3行第5列是通道(0)
    // 网格坐标(3,5)对应的世界坐标:
    // x = 列 * CELL_SIZE + CELL_SIZE/2 = 5 * 20 + 10 = 110
    // y = 行 * CELL_SIZE + CELL_SIZE/2 = 3 * 20 + 10 = 70
    position = QPointF(110.0f, 70.0f);

    // 设置玩家和碰撞框尺寸
    width = 18.0f;
    height = 30.0f;

    // 设置血量 - 确保一开始是满的
    maxHealth = 5;
    health = maxHealth;  // 确保血量是满的

    // 确保血量是满的，修复可能的初始化问题
    if (health != maxHealth) {
        health = maxHealth;
        qDebug() << "修复血量初始化问题，将血量设置为满值:" << health << "/" << maxHealth;
    }

    qDebug() << "玩家创建，尺寸:" << width << "x" << height;
    qDebug() << "玩家初始位置:(" << position.x() << "," << position.y() << ")";
    qDebug() << "玩家初始血量:" << health << "/" << maxHealth;

    // 检查边界框
    QRectF bbox = getBoundingBox();
    qDebug() << "玩家边界框:左上(" << bbox.left() << "," << bbox.top()
             << ") 右下(" << bbox.right() << "," << bbox.bottom() << ")";
}

QPointF Player::getPosition() const
{
    return position;
}

void Player::setPosition(float x, float y)
{
    if (position.x() != x || position.y() != y) {
        position.setX(x);
        position.setY(y);
        emit positionChanged();
    }
}

void Player::setPosition(const QPointF& pos)
{
    setPosition(pos.x(), pos.y());
}

void Player::moveUp()
{
    moveX = 0;
    moveY = -1;
    moving = true;
    qDebug() << "玩家: 向上移动";
}

void Player::moveDown()
{
    moveX = 0;
    moveY = 1;
    moving = true;
    qDebug() << "玩家: 向下移动";
}

void Player::moveLeft()
{
    moveX = -1;
    moveY = 0;
    moving = true;
    qDebug() << "玩家: 向左移动";
}

void Player::moveRight()
{
    moveX = 1;
    moveY = 0;
    moving = true;
    qDebug() << "玩家: 向右移动";
}

void Player::stop()
{
    moveX = 0;
    moveY = 0;
    moving = false;
    qDebug() << "玩家: 停止移动";
}

void Player::updatePosition()
{
    if (moving) {
        float newX = position.x() + moveX * speed;
        float newY = position.y() + moveY * speed;
        setPosition(newX, newY);
    }
}

bool Player::isMoving() const
{
    return moving;
}

float Player::getSpeed() const
{
    return speed;
}

void Player::setSpeed(float newSpeed)
{
    if (newSpeed > 0) {
        speed = newSpeed;
    }
}

QRectF Player::getBoundingBox() const
{
    return QRectF(
        position.x() - width/2,
        position.y() - height/2,
        width,
        height
    );
}

float Player::getWidth() const
{
    return width;
}

float Player::getHeight() const
{
    return height;
}

int Player::getHealth() const
{
    return health;
}

int Player::getMaxHealth() const
{
    return maxHealth;
}

void Player::setHealth(int newHealth)
{
    if (newHealth >= 0 && newHealth <= maxHealth && newHealth != health) {
        health = newHealth;
        emit healthChanged(health, maxHealth);

        if (health <= 0) {
            emit playerDied();
        }
    }
}

void Player::setMaxHealth(int newMaxHealth)
{
    if (newMaxHealth > 0 && newMaxHealth != maxHealth) {
        maxHealth = newMaxHealth;
        if (health > maxHealth) {
            health = maxHealth;
        }
        emit healthChanged(health, maxHealth);
    }
}

void Player::takeDamage(int damage)
{
    if (damage > 0) {
        int newHealth = health - damage;
        if (newHealth < 0) newHealth = 0;
        setHealth(newHealth);
        qDebug() << "玩家受到伤害:" << damage << "点，剩余血量:" << newHealth;
    }
}

void Player::heal(int amount)
{
    if (amount > 0) {
        int newHealth = health + amount;
        if (newHealth > maxHealth) newHealth = maxHealth;
        setHealth(newHealth);
    }
}

bool Player::isDead() const
{
    return health <= 0;
}
