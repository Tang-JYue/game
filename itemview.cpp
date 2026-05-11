#include "itemview.h"
#include <QPainter>
#include <QDebug>
#include <QFile>

ItemView::ItemView(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setAutoFillBackground(false);
}

ItemView::~ItemView()
{
    // 注意：我们不删除items，它们由GameController管理
}

void ItemView::setItems(const QVector<Item*>& itemsList)
{
    items = itemsList;
    update();
}

void ItemView::updateView()
{
    update();
}

QPixmap ItemView::loadImage(const QString& path)
{
    if (QFile::exists(path)) {
        QPixmap pixmap;
        if (pixmap.load(path)) {
            // 改为30x30大小
            return pixmap.scaled(30, 30, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
    }
    return QPixmap(); // 返回空图片
}

QPixmap ItemView::createDefaultImage(int type, int score)
{
    // 改为30x30
    QPixmap pixmap(30, 30);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // 根据类型使用不同颜色
    QColor colors[8] = {
        QColor(255, 100, 100),    // 红色-1分
        QColor(255, 150, 100),    // 橙色-2分
        QColor(255, 200, 100),    // 橙黄色-3分
        QColor(255, 255, 100),    // 黄色-5分
        QColor(200, 255, 100),    // 黄绿色-8分
        QColor(100, 255, 100),    // 绿色-10分
        QColor(100, 200, 255),    // 浅蓝色-15分
        QColor(100, 100, 255)     // 蓝色-20分
    };

    QColor color = colors[type % 8];

    // 绘制圆形物品
    painter.setBrush(color);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawEllipse(3, 3, 24, 24);

    // 绘制分数
    painter.setPen(Qt::black);
    QFont font = painter.font();
    font.setPointSize(10);
    font.setBold(true);
    painter.setFont(font);

    QString scoreText = QString::number(score);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, scoreText);

    return pixmap;
}

void ItemView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    if (items.isEmpty()) return;

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    for (Item *item : items) {
        if (item && !item->isCollected()) {
            QPointF pos = item->getPosition();
            int type = item->getType();
            int score = item->getScore();

            // 计算绘制位置(以左上角为基准) - 改为30x30
            float drawX = pos.x() - 15;
            float drawY = pos.y() - 15;

            // 加载图片
            QString imagePath = item->getImagePath();
            QPixmap pixmap = loadImage(imagePath);

            if (pixmap.isNull()) {
                // 如果图片加载失败，使用默认图片
                pixmap = createDefaultImage(type, score);
            }

            if (!pixmap.isNull()) {
                // 绘制30x30图片
                painter.drawPixmap(drawX, drawY, 30, 30, pixmap);
            }

            // 如果显示提示，绘制一个发光效果
            if (item->isShowPrompt()) {
                painter.setPen(QPen(QColor(255, 255, 0, 100), 3));
                painter.setBrush(Qt::NoBrush);
                painter.drawEllipse(pos.x() - 18, pos.y() - 18, 36, 36);
            }
        }
    }
}
