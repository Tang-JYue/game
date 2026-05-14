#include "healthview.h"
#include "player.h"
#include "gamecontroller.h"
#include <QPainter>
#include <QLinearGradient>

HealthView::HealthView(QWidget *parent) : QWidget(parent)
{
    setFixedHeight(80);  // 增加高度以适应更大的字体
    setStyleSheet("background-color: rgba(30, 30, 30, 240); border-bottom: 2px solid #555;");
}

void HealthView::setPlayer(Player *p)
{
    player = p;
    if (player) {
        // 立即获取玩家数据
        currentHealth = player->getHealth();
        maxHealth = player->getMaxHealth();
    }
    update();
}

void HealthView::setGameController(GameController *controller)
{
    gameController = controller;
    if (gameController) {
        // 立即获取游戏数据
        currentScore = gameController->getTotalScore();
        itemsCollected = gameController->getItemsCollected();
    }
    update();
}

void HealthView::updateDisplay()
{
    if (player) {
        currentHealth = player->getHealth();
        maxHealth = player->getMaxHealth();
    }
    if (gameController) {
        currentScore = gameController->getTotalScore();
        itemsCollected = gameController->getItemsCollected();
    }
    update();
}

void HealthView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制半透明背景
    painter.fillRect(rect(), QColor(30, 30, 30, 240));

    int windowWidth = width();
    int sectionWidth = windowWidth / 3;  // 将窗口分为三部分

    // 设置大字体
    QFont font;
    font.setPixelSize(18);
    font.setBold(true);
    font.setFamily("SimHei");
    painter.setFont(font);

    int startY = 20;

    // 1. 血量部分（左侧1/3）
    int healthSectionX = 20;

    // 绘制"血量"文字
    painter.setPen(Qt::white);
    painter.drawText(healthSectionX, startY + 20, "血量");

    // 血条背景
    int barX = healthSectionX + 60;  // "血量"文字后留出空间
    int barY = startY;
    int barWidth = sectionWidth - 120;
    int barHeight = 25;

    painter.setBrush(QColor(60, 60, 60));
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(barX, barY, barWidth, barHeight, 5, 5);

    // 计算血量百分比
    float healthPercent = 0.0f;
    if (maxHealth > 0) {
        healthPercent = (float)currentHealth / maxHealth;
    }

    int healthWidth = (int)(barWidth * healthPercent);

    // 血量颜色渐变
    QLinearGradient gradient(barX, barY, barX + healthWidth, barY);
    if (healthPercent > 0.6) {
        gradient.setColorAt(0, QColor(0, 200, 0));    // 绿色
        gradient.setColorAt(1, QColor(0, 150, 0));
    } else if (healthPercent > 0.3) {
        gradient.setColorAt(0, QColor(255, 180, 0));  // 橙色
        gradient.setColorAt(1, QColor(200, 140, 0));
    } else {
        gradient.setColorAt(0, QColor(255, 50, 50));  // 红色
        gradient.setColorAt(1, QColor(200, 30, 30));
    }

    // 绘制血量
    painter.setBrush(gradient);
    painter.drawRoundedRect(barX, barY, healthWidth, barHeight, 5, 5);

    // 血条边框
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(QColor(150, 150, 150), 2));
    painter.drawRoundedRect(barX, barY, barWidth, barHeight, 5, 5);

    // 血量数值
    painter.setPen(Qt::white);
    QString healthText = QString("%1/%2").arg(currentHealth).arg(maxHealth);
    painter.drawText(QRect(barX, barY, barWidth, barHeight), Qt::AlignCenter, healthText);

    // 2. 当前分数部分（中间1/3）
    int scoreSectionX = sectionWidth;

    // 绘制分数
    QFont scoreFont = font;
    scoreFont.setPixelSize(22);
    painter.setFont(scoreFont);

    painter.setPen(QColor(255, 255, 100));  // 黄色
    QString scoreText = QString("分数: %1").arg(currentScore);
    painter.drawText(scoreSectionX, startY + 30, scoreText);

    // 3. 目标分数部分（右侧1/3）
    int targetSectionX = sectionWidth * 2;

    painter.setPen(QColor(100, 255, 100));  // 绿色
    QString targetText = QString("目标: 612");
    painter.drawText(targetSectionX, startY + 30, targetText);

    // 绘制分隔线
    painter.setPen(QPen(QColor(100, 100, 100), 1));
    painter.drawLine(sectionWidth, 10, sectionWidth, height() - 10);
    painter.drawLine(sectionWidth * 2, 10, sectionWidth * 2, height() - 10);
}
