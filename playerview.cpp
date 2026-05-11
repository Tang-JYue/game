#include "playerview.h"
#include "player.h"
#include <QPainter>
#include <QDebug>

PlayerView::PlayerView(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
    createDefaultPlayerImage();
}

void PlayerView::setPlayer(Player* p)
{
    player = p;
    update();
}

void PlayerView::setImage(const QString& imagePath)
{
    if (!imagePath.isEmpty()) {
        QPixmap tempPixmap;
        if (tempPixmap.load(imagePath)) {
            // 加载成功，使用原始图片
            playerImage = tempPixmap;
            qDebug() << "玩家图片加载成功:" << imagePath;
        } else {
            // 加载失败，使用默认图片
            createDefaultPlayerImage();
            qDebug() << "玩家图片加载失败，使用默认图片";
        }
    } else {
        createDefaultPlayerImage();
        qDebug() << "玩家图片路径为空，使用默认图片";
    }
    update();
}

void PlayerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if (!player) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPointF pos = player->getPosition();
    float width = 18.0f;
    float height = 30.0f;

    // 计算绘制位置
    float drawX = pos.x() - width/2;
    float drawY = pos.y() - height/2;

    painter.drawPixmap(drawX, drawY, width, height, playerImage);
}

void PlayerView::createDefaultPlayerImage()
{
    playerImage = QPixmap(18, 30);
    playerImage.fill(Qt::transparent);

    QPainter painter(&playerImage);
    painter.setRenderHint(QPainter::Antialiasing);

    // 绘制黄色矩形
    painter.setBrush(Qt::yellow);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRoundedRect(0, 0, 20, 40, 5, 5);

    // 眼睛
    painter.setBrush(Qt::black);
    painter.drawEllipse(5, 10, 3, 3);
    painter.drawEllipse(12, 10, 3, 3);

    // 嘴巴
    painter.setPen(QPen(Qt::black, 2));
    painter.drawArc(5, 20, 10, 10, 180 * 16, 180 * 16);
}
