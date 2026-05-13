#ifndef FINALSTORYVIEW_H
#define FINALSTORYVIEW_H

#include <QDialog>

class QTextBrowser;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class FinalStoryView : public QDialog
{
    Q_OBJECT

public:
    // 构造函数不再接收参数
    explicit FinalStoryView(QWidget *parent = nullptr);
    ~FinalStoryView();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    void loadBackgroundImage();  // 加载背景图
    void loadStoryText();        // 加载故事文本

    // UI控件
    QTextBrowser *m_textBrowser;   // 文本区域
    QPushButton *m_closeButton;    // 关闭按钮
    QVBoxLayout *m_mainLayout;     // 主布局

    // 数据成员
    QString m_storyText;          // 故事文本
    QString m_backgroundPath;     // 背景图路径

    // 背景图
    QPixmap m_backgroundPixmap;
};

#endif // FINALSTORYVIEW_H
