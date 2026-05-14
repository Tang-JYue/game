#include "failview.h"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QDebug>
#include <QPixmap>
#include <QFont>
#include <QPainter>
#include <QGraphicsDropShadowEffect>

FailView::FailView(QWidget *parent)
    : QDialog(parent)
{
    // 设置窗口属性
    this->setWindowTitle("游戏结束");
    this->setFixedSize(900, 600);  // 较大的窗口

    // 设置无边框窗口，沉浸式体验
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // 加载背景图
    loadBackgroundImage();

    // 设置UI
    setupUI();
}

void FailView::setupUI()
{
    // 1. 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    // 2. 添加顶部弹性空间
    m_mainLayout->addStretch(2);

    // 3. 创建标题区域
    QHBoxLayout *titleLayout = new QHBoxLayout();
    titleLayout->addStretch();

    m_titleLabel = new QLabel(m_titleText, this);

    // 设置标题字体
    QFont titleFont;
    titleFont.setPointSize(20);
    titleFont.setFamily("SimHei");
    titleFont.setBold(true);
    m_titleLabel->setFont(titleFont);

    m_titleLabel->setStyleSheet(
        "color: #FFB6C1;"
        "background-color: rgba(0, 0, 0, 0);"
    );

    // 为标题添加阴影
    QGraphicsDropShadowEffect *titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(20);
    titleShadow->setColor(QColor(0, 0, 0, 150));
    titleShadow->setOffset(5, 5);
    m_titleLabel->setGraphicsEffect(titleShadow);

    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();

    m_mainLayout->addLayout(titleLayout);

    // 4. 添加中间弹性空间
    m_mainLayout->addStretch(1);

    // 5. 创建消息区域
    QHBoxLayout *messageLayout = new QHBoxLayout();
    messageLayout->addStretch();

    m_messageLabel = new QLabel(m_messageText, this);

    // 设置消息字体
    QFont messageFont;
    messageFont.setPointSize(14);
    messageFont.setFamily("SimHei");
    m_messageLabel->setFont(messageFont);

    m_messageLabel->setStyleSheet(
        "color: black;"
        "background-color: rgba(0, 0, 0, 0);"
        "padding: 20px;"
    );

    m_messageLabel->setAlignment(Qt::AlignCenter);

    // 为消息添加阴影
    QGraphicsDropShadowEffect *messageShadow = new QGraphicsDropShadowEffect();
    messageShadow->setBlurRadius(15);
    messageShadow->setColor(QColor(0, 0, 0, 100));
    messageShadow->setOffset(3, 3);
    m_messageLabel->setGraphicsEffect(messageShadow);

    messageLayout->addWidget(m_messageLabel);
    messageLayout->addStretch();

    m_mainLayout->addLayout(messageLayout);

    // 6. 添加中间弹性空间
    m_mainLayout->addStretch(2);

    // 7. 创建按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(140);  // 按钮间距
    buttonLayout->addStretch();

    // 7.1 重新开始按钮
    m_restartButton = new QPushButton("restart", this);
    m_restartButton->setFixedSize(180, 60);

    m_restartButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255, 255, 255, 180);"  // 浅白色
        "  color: #2c3e50;"
        "  border: 2px solid rgba(200, 200, 200, 200);"
        "  border-radius: 8px;"
        "  font-size: 22px;"
        "  font-weight: bold;"
        "  padding: 12px 25px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(240, 240, 240, 200);"
        "  border: 2px solid rgba(180, 180, 180, 220);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(230, 230, 230, 220);"
        "  border: 2px solid rgba(160, 160, 160, 240);"
        "}"
    );

    // 为按钮添加阴影
    QGraphicsDropShadowEffect *restartShadow = new QGraphicsDropShadowEffect();
    restartShadow->setBlurRadius(15);
    restartShadow->setColor(QColor(0, 0, 0, 100));
    restartShadow->setOffset(3, 3);
    m_restartButton->setGraphicsEffect(restartShadow);

    // 7.2 退出按钮
    m_quitButton = new QPushButton("off", this);
    m_quitButton->setFixedSize(180, 60);

    m_quitButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255, 255, 255, 180);"  // 浅白色
        "  color: #2c3e50;"
        "  border: 2px solid rgba(200, 200, 200, 200);"
        "  border-radius: 8px;"
        "  font-size: 22px;"
        "  font-weight: bold;"
        "  padding: 12px 25px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(240, 240, 240, 200);"
        "  border: 2px solid rgba(180, 180, 180, 220);"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(230, 230, 230, 220);"
        "  border: 2px solid rgba(160, 160, 160, 240);"
        "}"
    );

    QGraphicsDropShadowEffect *quitShadow = new QGraphicsDropShadowEffect();
    quitShadow->setBlurRadius(15);
    quitShadow->setColor(QColor(0, 0, 0, 100));
    quitShadow->setOffset(3, 3);
    m_quitButton->setGraphicsEffect(quitShadow);

    buttonLayout->addWidget(m_restartButton);
    buttonLayout->addWidget(m_quitButton);
    buttonLayout->addStretch();

    m_mainLayout->addLayout(buttonLayout);

    // 8. 添加底部弹性空间
    m_mainLayout->addStretch(2);

    this->setLayout(m_mainLayout);

    // 9. 连接按钮信号
    connect(m_restartButton, &QPushButton::clicked, this, &FailView::onRestartClicked);
    connect(m_quitButton, &QPushButton::clicked, this, &FailView::onQuitClicked);
}

void FailView::onRestartClicked()
{
    this->accept();  // 只调用 accept()
}

void FailView::onQuitClicked()
{
    this->reject();  // 只调用 reject()
}

void FailView::loadBackgroundImage()
{
    if (QFile::exists(m_backgroundPath)) {
        if (m_backgroundPixmap.load(m_backgroundPath)) {
            m_backgroundPixmap = m_backgroundPixmap.scaled(
                this->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            );

            // 调整背景图透明度
            QPixmap transparentPixmap(m_backgroundPixmap.size());
            transparentPixmap.fill(Qt::transparent);

            QPainter pixmapPainter(&transparentPixmap);
            pixmapPainter.setOpacity(0.7);  // 70%透明度
            pixmapPainter.drawPixmap(0, 0, m_backgroundPixmap);
            pixmapPainter.end();

            m_backgroundPixmap = transparentPixmap;

            qDebug() << "失败窗口背景图加载成功:" << m_backgroundPath;
        } else {
            qWarning() << "无法加载失败窗口背景图:" << m_backgroundPath;
        }
    } else {
        qWarning() << "失败窗口背景图文件不存在:" << m_backgroundPath;
        // 使用默认背景
        m_backgroundPath = ":/new/prefix1/background3.png";  // 回退到可用背景
        if (m_backgroundPixmap.load(m_backgroundPath)) {
            m_backgroundPixmap = m_backgroundPixmap.scaled(
                this->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            );
        }
    }
}

void FailView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (!m_backgroundPixmap.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), m_backgroundPixmap);
    } else {
        // 备用背景色
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(40, 40, 40));  // 深色
        gradient.setColorAt(1, QColor(20, 20, 20));  // 更深色
        painter.fillRect(rect(), gradient);
    }

    QDialog::paintEvent(event);
}

FailView::~FailView()
{
}
