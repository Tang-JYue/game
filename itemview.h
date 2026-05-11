#ifndef ITEMVIEW_H
#define ITEMVIEW_H

#include <QWidget>
#include <QVector>
#include "item.h"

class ItemView : public QWidget
{
    Q_OBJECT

public:
    explicit ItemView(QWidget *parent = nullptr);
    ~ItemView();

    void setItems(const QVector<Item*>& items);
    void updateView();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QVector<Item*> items;

    // 加载图片
    QPixmap loadImage(const QString& path);

    // 创建默认图片
    QPixmap createDefaultImage(int type, int score);
};

#endif // ITEMVIEW_H
