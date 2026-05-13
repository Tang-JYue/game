#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QDialog>
#include <QPixmap>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class StartWindow : public QDialog
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow();

signals:
    void startButtonClicked();  // 开始游戏信号
    void quitButtonClicked();   // 退出游戏信号

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onStartButtonClicked();
    void onQuitButtonClicked();

private:
    void loadBackgroundImage();

    // UI控件
    QLabel *m_titleLabel;        // LPgame标题
    QPushButton *m_startButton;  // 开始游戏按钮
    QPushButton *m_quitButton;   // 退出游戏按钮
    QVBoxLayout *m_mainLayout;   // 主布局

    // 背景图
    QPixmap m_backgroundPixmap;
};

#endif // STARTWINDOW_H
