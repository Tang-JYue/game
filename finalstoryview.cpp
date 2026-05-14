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
        "  —— Rose, Labyrinth, Home · 完结 ——";

    // 2. 设置窗口属性
    this->setWindowTitle("大结局 - 回归");
    this->setFixedSize(1200, 850);  // 适当增大窗口

    // 3. 加载背景图
    loadBackgroundImage();

    // 4. 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 5. 创建文本区域
    m_textBrowser = new QTextBrowser(this);
    m_textBrowser->setMinimumSize(1200, 700);

    // 6. 设置字体
    QFont textFont;
    textFont.setPointSize(13);
    textFont.setFamily("SimHei");
    m_textBrowser->setFont(textFont);

    // 7. 设置文本区域样式
    m_textBrowser->setStyleSheet(
        "QTextBrowser {"
        "  border: none;"
        "  background-color: transparent;"
        "  color: black;"
        "  padding: 50px 80px;"
        "  line-height: 1.8;"
        "}"
    );

    // 8. 禁用滚动条
    m_textBrowser->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_textBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 9. 设置文本格式
    m_textBrowser->setPlainText(m_storyText);
    m_textBrowser->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_textBrowser->setWordWrapMode(QTextOption::WordWrap);
    m_textBrowser->setLineWrapMode(QTextBrowser::WidgetWidth);

    // 10. 为最后一行设置特殊格式（花体字）
    QTextCursor cursor = m_textBrowser->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.movePosition(QTextCursor::StartOfLine, QTextCursor::KeepAnchor);

    QTextCharFormat format;
    QFont font;
    font.setFamily("Brush Script MT");
    font.setPointSize(16);
    font.setItalic(true);
    format.setFont(font);
    format.setForeground(QColor("#e8a0c8"));  // 淡粉色
    cursor.mergeCharFormat(format);

    // 11. 为文字添加阴影效果
    QGraphicsDropShadowEffect *textShadow = new QGraphicsDropShadowEffect();
    textShadow->setBlurRadius(10);
    textShadow->setColor(QColor(255, 255, 255, 150));
    textShadow->setOffset(2, 2);
    m_textBrowser->setGraphicsEffect(textShadow);

    m_mainLayout->addWidget(m_textBrowser, 1);

    // 12. 创建底部按钮区域
       QWidget *buttonContainer = new QWidget(this);
       buttonContainer->setFixedHeight(100);  // 固定按钮区域高度
       buttonContainer->setStyleSheet(
           "background-color: rgba(0, 0, 0, 0);"  // 完全透明背景
       );

       QHBoxLayout *buttonLayout = new QHBoxLayout(buttonContainer);
       buttonLayout->setContentsMargins(40, 0, 40, 20);  // 底部边距20px
       buttonLayout->setSpacing(150);  // 按钮间距

       // 添加弹性空间使按钮居中
       buttonLayout->addStretch();

       // 创建restart按钮
       m_restartButton = new QPushButton("restart", buttonContainer);
       m_restartButton->setFixedSize(150, 50);

       // 创建off按钮
       m_quitButton = new QPushButton("off", buttonContainer);
       m_quitButton->setFixedSize(150, 50);

       // 设置按钮样式为浅白色框
       QString buttonStyle =
           "QPushButton {"
           "  background-color: rgba(255, 255, 255, 180);"  // 浅白色背景
           "  color: #2c3e50;"                              // 深灰色文字
           "  border: 2px solid rgba(200, 200, 200, 200);"  // 浅灰色边框
           "  border-radius: 8px;"
           "  font-size: 25px;"
           "  font-weight: bold;"
           "  padding: 10px 20px;"
           "}"
           "QPushButton:hover {"
           "  background-color: rgba(240, 240, 240, 200);"
           "  border: 2px solid rgba(180, 180, 180, 220);"
           "}"
           "QPushButton:pressed {"
           "  background-color: rgba(230, 230, 230, 220);"
           "  border: 2px solid rgba(160, 160, 160, 240);"
           "}";

       m_restartButton->setStyleSheet(buttonStyle);
       m_quitButton->setStyleSheet(buttonStyle);

       // 将按钮添加到布局
       buttonLayout->addWidget(m_restartButton);
       buttonLayout->addWidget(m_quitButton);

       buttonLayout->addStretch();

       // 13. 将按钮容器添加到主布局
       m_mainLayout->addWidget(buttonContainer, 0, Qt::AlignBottom);

       this->setLayout(m_mainLayout);

       // 14. 连接按钮信号
       connect(m_restartButton, &QPushButton::clicked, this, &FinalStoryView::accept);
       connect(m_quitButton, &QPushButton::clicked, this, &FinalStoryView::reject);

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
