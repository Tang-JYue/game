#ifndef HEALTHVIEW_H
#define HEALTHVIEW_H

#include <QWidget>

class Player;
class GameController;

class HealthView : public QWidget
{
    Q_OBJECT

public:
    explicit HealthView(QWidget *parent = nullptr);

    void setPlayer(Player* player);
    void setGameController(GameController* controller);

public slots:
    void updateDisplay();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    Player* player = nullptr;
    GameController* gameController = nullptr;
    int currentHealth = 0;
    int maxHealth = 0;
    int currentScore = 0;
    int itemsCollected = 0;
};

#endif // HEALTHVIEW_H
