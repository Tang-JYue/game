#include "player.h"
quint64 Player::s_nextInstanceId = 1;

Player::Player(QObject *parent) : QObject(parent)
{
    //为新玩家分配唯一ID
       m_instanceId = s_nextInstanceId++;

    // 初始位置 - 在迷宫中找到一个安全的起始位置
    // 检查迷宫数据，找到第3行第5列是通道(0)
    // 网格坐标(3,5)对应的世界坐标:
    // x = 列 * CELL_SIZE + CELL_SIZE/2 = 5 * 20 + 10 = 110
    // y = 行 * CELL_SIZE + CELL_SIZE/2 = 3 * 20 + 10 = 70
    position = QPointF(110.0f, 70.0f);

    // 设置玩家和碰撞框尺寸
    width = 18.0f;
    height = 30.0f;

    // 设置血量
    maxHealth = 5;
    health = maxHealth;
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
}

void Player::moveDown()
{
    moveX = 0;
    moveY = 1;
    moving = true;
}

void Player::moveLeft()
{
    moveX = -1;
    moveY = 0;
    moving = true;
}

void Player::moveRight()
{
    moveX = 1;
    moveY = 0;
    moving = true;
}

void Player::stop()
{
    moveX = 0;
    moveY = 0;
    moving = false;
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
//实现获取ID的函数
quint64 Player::getInstanceId() const
{
    return m_instanceId;
}
