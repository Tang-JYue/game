#include "finalstoryview.h"

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
#include <QScrollBar>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <QGraphicsDropShadowEffect>

FinalStoryView::FinalStoryView(QWidget *parent)
    : QDialog(parent)
{
    // 1. 初始化数据
    m_backgroundPath = ":/new/prefix1/background2.png";
    m_storyText =
        "恭喜你成功达到612分！\n\n"
        "我想起来了。\n\n"
        "我是小王子，来自B-612星球。\n\n"
        "我有三座火山，两座是活火山，一座是死火山。\n\n"
        "我还有一朵玫瑰，她有着四根刺。\n"
        "她说她是宇宙中唯一的一朵玫瑰，\n"
        "于是我离开了，想看看外面的世界。\n\n"
        "我拜访了好几个小行星：\n\n"
        "一颗星球上住着一位国王。\n"
        "他身披紫色披风，命令太阳在正确的时间升起和落下。\n\n"
        "另一颗星球上住着一位商人。\n"
        "他忙着数星星，把数字写在账本上，锁在抽屉里。\n\n"
        "还有一颗星球很小很小，上面只有一盏路灯和一个点灯人。\n"
        "这个星球每分钟转一圈，点灯人没有休息的时间。\n\n"
        "然后我来到了地球。\n\n"
        "在地球上，我遇到了狐狸。\n"
        "他对我说：\"重要的东西用眼睛是看不见的，要用心去看。\"\n\n"
        "再后来，我遇到了一位飞行员。\n"
        "他给我看他六岁时画的画，一条蟒蛇吞下了一头大象。\n\n"
        "我想念我的玫瑰了。\n\n"
        "蛇说它能帮我回家。\n"
        "它咬了我，不痛，像被针扎了一下。\n\n"
        "然后我就在这个迷宫里了，\n"
        "忘记了我是谁，忘记了我要去哪里。\n\n\n"
        "感谢坐在电脑前的你，帮我找回了我的记忆\n\n"
        "我现在已经回到了自己的星球。\n"
        "三座火山静静矗立，而玫瑰，正在玻璃罩下安然无恙。\n\n\n"
        "重要的东西，眼睛是看不见的。\n\n"
        "达成结局：回家\n\n\n"
        "—— 故事收集游戏 · 完结 ——";

    // 2. 设置窗口属性
    this->setWindowTitle("大结局 - 小王子的回归");
    this->setFixedSize(1200, 800);

    // 3. 加载背景图
    loadBackgroundImage();

    // 4. 设置对话框为无边框，实现更沉浸式的体验
    this->setWindowFlags(this->windowFlags() | Qt::FramelessWindowHint);

    // 5. 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);  // 移除所有边距
    m_mainLayout->setSpacing(0);

    // 6. 创建文本区域
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setMinimumSize(1200, 700);

    // 设置QTextBrowser的字体
    QFont textFont;
    textFont.setPointSize(16);
    textFont.setFamily("Microsoft YaHei");
    textFont.setBold(true);
    textFont.setStyleStrategy(QFont::PreferAntialias);
    m_textBrowser->setFont(textFont);

    // 设置文本区域样式 - 完全透明，无边框，无滚动条
    m_textBrowser->setStyleSheet(
        "QTextBrowser {"
        "  border: none;"                     // 无边框
        "  background-color: transparent;"    // 完全透明
        "  color: black;"                    // 黑色文字
        "  padding: 100px 150px;"           // 增大内边距
        "  line-height: 1.6;"               // 增大行高
        "}"
    );

    // +++ 关键修改：禁用滚动条 +++
    m_textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);  // 始终关闭垂直滚动条
    m_textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 也关闭水平滚动条

    // 7. 设置文本格式
    m_textBrowser->setPlainText(m_storyText);
    m_textBrowser->setAlignment(Qt::AlignCenter);
    m_textBrowser->setWordWrapMode(QTextOption::WordWrap);
    m_textBrowser->setLineWrapMode(QTextBrowser::WidgetWidth);

    // 8. 为文字添加阴影效果
    QGraphicsDropShadowEffect *textShadow = new QGraphicsDropShadowEffect();
    textShadow->setBlurRadius(10);
    textShadow->setColor(QColor(255, 255, 255, 150));
    textShadow->setOffset(2, 2);
    m_textBrowser->setGraphicsEffect(textShadow);

    // 9. 创建关闭按钮区域
    // +++ 关键修改1：将文本区域添加到主布局，并设置拉伸因子 +++
    // 参数1：拉伸因子为1，表示文本区域会占据所有可用空间
    m_mainLayout->addWidget(m_textBrowser, 1);

    // 2. 创建底部按钮区域
    // 创建一个容器widget来放置按钮，方便样式控制
    QWidget *buttonContainer = new QWidget(this);
    buttonContainer->setFixedHeight(100);  // 固定按钮区域高度
    buttonContainer->setStyleSheet(
        "background-color: rgba(0, 0, 0, 0);"  // 完全透明背景
    );

    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
    buttonLayout->setContentsMargins(0, 0, 0, 20);  // 底部边距20px
    buttonLayout->setSpacing(0);

    // 添加左侧弹性空间
    buttonLayout->addStretch();

    // 创建关闭按钮
    m_closeButton = new QPushButton("关闭并重新开始", buttonContainer);
    m_closeButton->setFixedSize(200, 50);

    m_closeButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(46, 204, 113, 180);"
        "  color: white;"
        "  border: 2px solid rgba(39, 174, 96, 200);"
        "  border-radius: 8px;"
        "  font-size: 18px;"
        "  font-weight: bold;"
        "  padding: 10px 30px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(39, 174, 96, 220);"
        "  border: 2px solid rgba(33, 150, 83, 220);"
        "}"
    );

    connect(m_closeButton, &QPushButton::clicked, this, &FinalStoryView::accept);

    buttonLayout->addWidget(m_closeButton);

    // 添加右侧弹性空间
    buttonLayout->addStretch();

    // +++ 关键修改2：将按钮容器添加到主布局，不设置拉伸因子 +++
    // 这样按钮区域会保持在底部，不会挤压文本区域
    m_mainLayout->addWidget(buttonContainer, 0, Qt::AlignBottom);

    this->setLayout(m_mainLayout);

    qDebug() << "FinalStoryView 创建完成，按钮已固定到底部";
}

// 加载背景图的函数
void FinalStoryView::loadBackgroundImage()
{
    if (QFile::exists(m_backgroundPath)) {
        if (m_backgroundPixmap.load(m_backgroundPath)) {
            m_backgroundPixmap = m_backgroundPixmap.scaled(
                this->size(),
                Qt::IgnoreAspectRatio,  // 忽略比例，填满整个窗口
                Qt::SmoothTransformation
            );

            qDebug() << "最终故事背景图加载成功:" << m_backgroundPath;
        } else {
            qWarning() << "无法加载最终故事背景图:" << m_backgroundPath;
        }
    } else {
        qWarning() << "最终故事背景图文件不存在:" << m_backgroundPath;
    }
}

// 重写绘制事件
void FinalStoryView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (!m_backgroundPixmap.isNull()) {
        // 直接绘制背景图，不添加透明度
        painter.drawPixmap(0, 0, width(), height(), m_backgroundPixmap);
    } else {
        // 备用背景色
        painter.fillRect(rect(), QColor(240, 240, 240));
    }

    QDialog::paintEvent(event);
}

FinalStoryView::~FinalStoryView()
{
    // 自动清理
}
