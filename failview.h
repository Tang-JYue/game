#ifndef FAILVIEW_H
#define FAILVIEW_H

#include <QDialog>
#include <QPixmap>

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class FailView : public QDialog
{
    Q_OBJECT

public:
    explicit FailView(QWidget *parent = nullptr);
    ~FailView();

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onRestartClicked();
    void onQuitClicked();

private:
    void loadBackgroundImage();
    void setupUI();

    // UI控件
    QLabel *m_titleLabel;        // 失败标题
    QLabel *m_messageLabel;      // 失败信息
    QPushButton *m_restartButton; // 重新开始按钮
    QPushButton *m_quitButton;    // 退出按钮
    QVBoxLayout *m_mainLayout;    // 主布局

    // 背景图
    QPixmap m_backgroundPixmap;
    QString m_backgroundPath = ":/new/prefix1/background3.png"; // 背景图路径

    // 文本内容
    QString m_titleText = "游戏结束";
    QString m_messageText =
        "很遗憾，你的生命值已归零。\n\n"
        "你将永远留在这个迷宫中\n\n"
        "达成结局：迷宫中的幽灵";
};

#endif // FAILVIEW_H
