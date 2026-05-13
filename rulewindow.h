#ifndef RULEWINDOW_H
#define RULEWINDOW_H

#include <QDialog>
#include <QPixmap>

class QTextBrowser;
class QPushButton;
class QVBoxLayout;

class RuleWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RuleWindow(QWidget *parent = nullptr);
    ~RuleWindow();

signals:
    void continueButtonClicked();  // 继续游戏信号

protected:
    void paintEvent(QPaintEvent *event) override;

private slots:
    void onContinueButtonClicked();

private:
    void loadBackgroundImage();

    // UI控件
    QTextBrowser *m_ruleText;      // 规则文本
    QPushButton *m_continueButton; // 继续按钮
    QVBoxLayout *m_mainLayout;     // 主布局

    // 背景图
    QPixmap m_backgroundPixmap;
};

#endif // RULEWINDOW_H
