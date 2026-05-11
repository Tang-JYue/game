#ifndef PLAYERVIEW_H
#define PLAYERVIEW_H

#include <QWidget>
#include <QPixmap>

class Player;

class PlayerView : public QWidget
{
    Q_OBJECT

public:
    explicit PlayerView(QWidget *parent = nullptr);

    void setPlayer(Player* player);
    void setImage(const QString& imagePath = "");

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Player* player = nullptr;
    QPixmap playerImage;

    void createDefaultPlayerImage();
};

#endif // PLAYERVIEW_H
