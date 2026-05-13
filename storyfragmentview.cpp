#include "storyfragmentview.h"

// 必要的Qt头文件
#include <QLabel>
#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QPainter>

StoryFragmentView::StoryFragmentView(const QString &storyText, const QString &imagePath, QWidget *parent)
    : QDialog(parent)
{
    // 设置窗口属性
    this->setWindowTitle("故事碎片");
    this->setFixedSize(1000, 500);  // 调整为更宽的窗口以适应1:2比例
    this->setModal(true);           // 设置为模态对话框

    //  加载背景图
    loadBackgroundImage();

    // 1. 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(15, 15, 15, 15); // 设置内边距
    m_mainLayout->setSpacing(0);                     // 移除组件间默认间距

    // 2. 创建内容区域（水平布局：左图右文，1:2比例）
    m_contentLayout = new QHBoxLayout();
    m_contentLayout->setSpacing(20); // 图片和文本之间的间距

    // 2.1 左侧：图片区域 (占比约1/3)
    m_imageLabel = new QLabel(this);
    m_imageLabel->setFixedSize(320, 350); // 固定图片显示区域大小
    m_imageLabel->setAlignment(Qt::AlignCenter);
    // 设置半透明背景，使背景图能透出来
    m_imageLabel->setStyleSheet(
        "QLabel {"
        "  background-color: rgba(255, 255, 255, 180);"  // 半透明白色背景
        "  border: 2px solid rgba(200, 200, 200, 150);"  // 半透明边框
        "  border-radius: 8px;"                          // 圆角
        "}"
    );
    m_contentLayout->addWidget(m_imageLabel, 1); // 权重为1

    // 2.2 右侧：文本区域 (占比约2/3)
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setMinimumSize(640, 350); // 文本区域更宽
    m_textBrowser->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    // 设置字体
    QFont textFont;
    textFont.setPointSize(10);               // 字号
    textFont.setFamily("Microsoft YaHei");   // 使用更清晰的字体
    m_textBrowser->setFont(textFont);

    // 设置文本区域样式：半透明背景，圆角边框
    m_textBrowser->setStyleSheet(
        "QTextBrowser {"
        "  border: 1px solid rgba(150, 150, 150, 100);"  // 半透明边框
        "  border-radius: 8px;"                          // 圆角
        "  background-color: rgba(255, 255, 255, 200);"  // 半透明白色背景
        "  padding: 20px;"                               // 内边距
        "  line-height: 1.0;"                            // 行高
        "}"
    );

    // 处理文本：替换错误的/n为正确的换行符\n，并设置文本
    QString formattedText = storyText;
    formattedText.replace("/n", "\n");
    m_textBrowser->setPlainText(formattedText);

    m_contentLayout->addWidget(m_textBrowser, 2); // 权重为2，实现1:2比例

    // 将内容布局添加到主布局
    m_mainLayout->addLayout(m_contentLayout);

    // 3. 创建关闭按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch(); // 左侧弹性空间

    m_closeButton = new QPushButton("关闭", this);
    m_closeButton->setFixedSize(90, 30); // 较小的关闭按钮

    // 设置按钮样式，使其在背景图上可见
    m_closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(93, 173, 226, 200);"  // 半透明浅蓝色背景
        "  color: white;"
        "  border: 1px solid rgba(52, 152, 219, 200);"  // 半透明边框
        "  border-radius: 4px;"                         // 圆角
        "  font-size: 12px;"
        "  font-weight: bold;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(52, 152, 219, 220);"  // 悬停时更深的半透明蓝色
        "}"
    );

    // 连接按钮的点击信号到对话框的accept槽（会关闭对话框）
    connect(m_closeButton, &QPushButton::clicked, this, &StoryFragmentView::accept);

    buttonLayout->addWidget(m_closeButton);
    buttonLayout->addStretch(); // 右侧弹性空间

    m_mainLayout->addLayout(buttonLayout);

    // 设置对话框的布局
    this->setLayout(m_mainLayout);

    // 4. 加载并显示故事图片
    loadImage(imagePath);
}

// 加载背景图的函数
void StoryFragmentView::loadBackgroundImage()
{
    // 背景图路径 - 请根据您的实际资源文件路径修改
    QString bgPath = ":/new/prefix1/background.png";

    if (QFile::exists(bgPath)) {
        if (m_backgroundPixmap.load(bgPath)) {
            // 可选：缩放背景图以适应窗口大小
            m_backgroundPixmap = m_backgroundPixmap.scaled(
                this->size(),
                Qt::KeepAspectRatioByExpanding,  // 保持比例，可能裁剪
                Qt::SmoothTransformation
            );
            qDebug() << "故事弹窗背景图加载成功:" << bgPath;
        } else {
            qWarning() << "无法加载故事弹窗背景图:" << bgPath;
        }
    } else {
        qWarning() << "故事弹窗背景图文件不存在:" << bgPath;
    }
}

//  重写绘制事件，绘制背景图
void StoryFragmentView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    // 如果背景图加载成功，绘制背景图
    if (!m_backgroundPixmap.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), m_backgroundPixmap);
    } else {
        // 如果没有背景图，使用渐变色背景
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(240, 248, 255));  // 浅蓝色
        gradient.setColorAt(1, QColor(200, 230, 255));  // 更深的浅蓝色
        painter.fillRect(rect(), gradient);
    }

    // 继续默认的绘制（控件等）
    QDialog::paintEvent(event);
}

// 加载故事图片的私有辅助函数
void StoryFragmentView::loadImage(const QString& imagePath)
{
    QPixmap pixmap;
    if (QFile::exists(imagePath) && pixmap.load(imagePath)) {
        // 缩放图片以适应标签大小，保持宽高比例，使用平滑变换
        pixmap = pixmap.scaled(m_imageLabel->size(),
                               Qt::KeepAspectRatio,
                               Qt::SmoothTransformation);
        m_imageLabel->setPixmap(pixmap);
    } else {
        // 图片加载失败时显示提示
        qWarning() << "无法加载故事图片：" << imagePath;
        m_imageLabel->setText("插图未找到");
        m_imageLabel->setStyleSheet(
            m_imageLabel->styleSheet() +  // 保持原有样式
            "font-size: 16px; "
            "color: #7f8c8d; "
            "qproperty-alignment: AlignCenter;"
        );
    }
}

StoryFragmentView::~StoryFragmentView()
{
    // Qt的对象树会自动管理QWidget子对象的销毁
    // 如果动态分配了非QObject对象，才需要在这里手动删除
}
