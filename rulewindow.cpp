#include "rulewindow.h"

#include <QTextBrowser>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

RuleWindow::RuleWindow(QWidget *parent)
    : QDialog(parent)
{
    // 设置窗口属性
    this->setWindowTitle("游戏规则");
    this->setFixedSize(900, 700);

    // 设置无边框窗口
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // 加载背景图
    loadBackgroundImage();

    // 1. 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(50, 50, 50, 50);
    m_mainLayout->setSpacing(30);

    // 2. 创建规则文本区域
    m_ruleText = new QTextBrowser(this);
    m_ruleText->setMinimumSize(800, 500);

    // 设置字体
    QFont textFont;
    textFont.setPointSize(11);
    textFont.setFamily("SimHei");
    m_ruleText->setFont(textFont);

    // 设置文本区域样式
    m_ruleText->setStyleSheet(
                "QTextBrowser {"
                  "  border: none;"                    // 移除边框
                  "  background-color: transparent;"   // 完全透明背景
                  "  color: black;"                   // 黑色文字（确保在背景上可见）
                  "  padding: 40px 50px;"             // 调整内边距，让文字不贴边
                  "  line-height: 1.5;"               // 增大行高，提高可读性
                  "}"
    );

    // 设置规则文本内容
    QString rules =
        // 主标题
        "<div style='text-align: center; margin-bottom: 40px;'>"
        "  <h1 style='color: white; font-size: 18pt; margin-bottom: 20px;"
        "     text-shadow: 3px 3px 6px rgba(0,0,0,0.8);'>"
        "    我是谁？这里是哪里？"
        "  </h1>"
        "</div>"

        // 正文内容
        "<div style='text-align: justify; font-size: 12pt;'>"
        "  <p style='margin-bottom: 25px;'>"
        "    你好，我是一个失去了记忆的人，"
        "  </p>"
        "  <p style='margin-bottom: 25px;'>"
        "    能不能请你帮我找寻我的记忆，"
        "  </p>"
        "  <p style='margin-bottom: 25px;'>"
        "    让我找到我的来处与归途"
        "  </p>"
        "</div>"

        // 花体标题
        "<div style='text-align: center; margin: 40px 0;'>"
        "  <h2 style='"
        "    font-family: 'Brush Script MT', 'Lucida Calligraphy', 'French Script MT', cursive;"
        "    font-size: 32pt;"
        "    color: #f8c8dc;"  // 淡粉色
        "    font-style: italic;"
        "    font-weight: bold;"
        "    text-shadow: 2px 2px 4px rgba(0,0,0,0.5);"
        "  '>"
        "    Rose, Labyrinth, Home"
        "  </h2>"
        "</div>"

        // 召唤语
        "<div style='text-align: center; margin: 30px 0;'>"
        "  <p style='font-size: 20pt; font-weight: bold; color: #f8f8a0;"
        "     text-shadow: 2px 2px 4px rgba(0,0,0,0.5);'>"
        "    请帮小王子找回自己的记忆！"
        "  </p>"
        "</div>"

        // 规则标题
        "<div style='text-align: left; margin: 40px 0 20px 0;'>"
        "  <h3 style='font-size: 18pt; color: black; font-weight: bold;'>"
        "    游戏规则如下："
        "  </h3>"
        "</div>"

        "<h2 style='color: #f8f8a0;'>🎮 游戏目标</h2>"
        "<p>在迷宫中探索，收集物品碎片，找回小王子的记忆，最终达到<strong>612分</strong>触发结局。</p>"

        "<h2 style='color: #f8f8a0;'>👤 移动控制</h2>"
        "<p>• <strong>W / ↑</strong>  向上移动</p>"
        "<p>• <strong>A / ←</strong>  向左移动</p>"
        "<p>• <strong>S / ↓</strong>  向下移动</p>"
        "<p>• <strong>D / →</strong>  向右移动</p>"

        "<h2 style='color: #f8f8a0;'>🎯 物品交互</h2>"
        "<p>• 靠近物品时，可以收集或阅读</p>"
        "<p>• <strong>C 键</strong> - 收集物品（获得分数）</p>"
        "<p>• <strong>R 键</strong> - 阅读物品背后的故事</p>"

        "<h2 style='color:#f8f8a0;'>❤️ 血量系统</h2>"
        "<p>• 撞到墙壁会<strong>减少1点生命值</strong></p>"
        "<p>• 生命值为0时游戏失败</p>"
        "<p>• 屏幕顶部有血量显示</p>"

        "<h2 style='color:#f8f8a0;'>📊 分数系统</h2>"
        "<p>• 每个物品价值<strong>12, 18, 25, 35, 48, 64, 85, 110分不等</strong></p>"
        "<p>• 达到612分后触发最终结局</p>"

        // 开始提示
        "<div style='text-align: center; margin-top: 50px;'>"
        "  <p style='font-size: 20pt; font-weight: bold; color: #f8f8a0;"
        "     text-shadow: 2px 2px 4px rgba(0,0,0,0.5);'>"
        "    现在，游戏开始！"
        "  </p>"
        "</div>";

    m_ruleText->setHtml(rules);
    m_ruleText->setAlignment(Qt::AlignTop);

    // 禁用滚动条
    m_ruleText->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_ruleText->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // 添加阴影效果
    QGraphicsDropShadowEffect *textShadow = new QGraphicsDropShadowEffect();
    textShadow->setBlurRadius(20);
    textShadow->setColor(QColor(0, 0, 0, 100));
    textShadow->setOffset(5, 5);
    m_ruleText->setGraphicsEffect(textShadow);

    m_mainLayout->addWidget(m_ruleText);

    // 3. 创建继续按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();

    m_continueButton = new QPushButton("continue", this);
    m_continueButton->setFixedSize(200, 60);

    m_continueButton->setStyleSheet(
                "QPushButton {"
                   "  background-color: rgba(255, 255, 255, 180);"  // 浅白色，180透明度
                   "  color: #2c3e50;"                              // 深灰色文字
                   "  border: 2px solid rgba(220, 220, 220, 200);"  // 更浅的灰色边框
                   "  border-radius: 8px;"                          // 稍微调整圆角
                   "  font-size: 20px;"                            // 调整字体大小
                   "  font-weight: bold;"
                   "  padding: 8px 25px;"
                   "}"
                   "QPushButton:hover {"
                   "  background-color: rgba(240, 240, 240, 200);"  // 悬停时稍微变深
                   "  border: 2px solid rgba(200, 200, 200, 220);"
                   "}"
                   "QPushButton:pressed {"
                   "  background-color: rgba(230, 230, 230, 220);"  // 按下时再深一点
                   "  border: 2px solid rgba(180, 180, 180, 240);"
                   "}"
    );

    // 添加阴影效果
    QGraphicsDropShadowEffect *buttonShadow = new QGraphicsDropShadowEffect();
    buttonShadow->setBlurRadius(15);
    buttonShadow->setColor(QColor(0, 0, 0, 100));
    buttonShadow->setOffset(3, 3);
    m_continueButton->setGraphicsEffect(buttonShadow);

    buttonLayout->addWidget(m_continueButton);
    buttonLayout->addStretch();

    m_mainLayout->addLayout(buttonLayout);

    this->setLayout(m_mainLayout);

    // 连接按钮信号
    connect(m_continueButton, &QPushButton::clicked, this, &RuleWindow::onContinueButtonClicked);
}

void RuleWindow::onContinueButtonClicked()
{
    emit continueButtonClicked();
}

void RuleWindow::loadBackgroundImage()
{
    QString bgPath = ":/new/prefix1/background3.png";

    if (QFile::exists(bgPath)) {
        if (m_backgroundPixmap.load(bgPath)) {
            m_backgroundPixmap = m_backgroundPixmap.scaled(
                this->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            );
        } else {
            qWarning() << "无法加载 RuleWindow 背景图:" << bgPath;
        }
    } else {
        qWarning() << "RuleWindow 背景图文件不存在:" << bgPath;
    }
}

void RuleWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (!m_backgroundPixmap.isNull()) {
        // 创建一个更透明的背景
        QPixmap transparentPixmap(m_backgroundPixmap.size());
        transparentPixmap.fill(Qt::transparent);

        QPainter pixmapPainter(&transparentPixmap);
        pixmapPainter.setOpacity(0.7);  // 70%透明度
        pixmapPainter.drawPixmap(0, 0, m_backgroundPixmap);
        pixmapPainter.end();

        painter.drawPixmap(0, 0, width(), height(), transparentPixmap);
    } else {
        // 备用背景色
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(240, 240, 240));
        gradient.setColorAt(1, QColor(220, 220, 220));
        painter.fillRect(rect(), gradient);
    }

    QDialog::paintEvent(event);
}

RuleWindow::~RuleWindow()
{
    // 自动清理
}
