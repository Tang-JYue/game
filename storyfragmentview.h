#ifndef STORYFRAGMENTVIEW_H
#define STORYFRAGMENTVIEW_H

#include <QDialog>

// 前向声明控件类，减少编译依赖
class QLabel;
class QTextBrowser;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;

class StoryFragmentView : public QDialog
{
    Q_OBJECT

public:
    // 构造函数：接收要显示的数据（文本和图片路径）
    explicit StoryFragmentView(const QString &storyText, const QString &imagePath, QWidget *parent = nullptr);
    ~StoryFragmentView();

protected:
    //  重写绘制事件以实现背景图
    void paintEvent(QPaintEvent *event) override;

private:
    // 加载并显示图片的辅助函数
    void loadImage(const QString& imagePath);

    //  加载背景图的函数
    void loadBackgroundImage();

    // UI控件指针
    QLabel *m_imageLabel;          // 左侧图片标签
    QTextBrowser *m_textBrowser;   // 右侧文本浏览器
    QPushButton *m_closeButton;    // 关闭按钮
    QVBoxLayout *m_mainLayout;     // 主垂直布局
    QHBoxLayout *m_contentLayout;  // 内容水平布局（左图右文）

    //  背景图成员变量
    QPixmap m_backgroundPixmap;
};

#endif // STORYFRAGMENTVIEW_H
