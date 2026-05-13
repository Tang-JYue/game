#include "startwindow.h"

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
#include <QFontDatabase>

StartWindow::StartWindow(QWidget *parent)
    : QDialog(parent)
{
    // 设置窗口属性
    this->setWindowTitle("Rose, Labyrinth, Home");
    this->setFixedSize(800, 600);

    // 设置无边框窗口
    this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);

    // 加载背景图
    loadBackgroundImage();

    // 1. 创建主布局
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(20, 40, 20, 40); // 增加上下边距到40px
    m_mainLayout->setSpacing(0);

    // 2. 在上方添加弹性空间，将整体内容向下推
    m_mainLayout->addStretch(6);

    // 3. 创建标题区域
    QHBoxLayout *topLayout = new QHBoxLayout();

    // 创建标题
    m_titleLabel = new QLabel("Rose, Labyrinth, Home", this);

    // 设置花体字
    QFont titleFont;
    QStringList fancyFonts = {
        "Brush Script MT",
        "Lucida Calligraphy",
        "French Script MT",
        "Edwardian Script ITC",
        "Segoe Script",
        "Comic Sans MS"
    };

    // 寻找可用的花体字体
    QString selectedFont = "Arial";
    QFontDatabase fontDb;
    for (const QString &fontName : fancyFonts) {
        if (fontDb.families().contains(fontName)) {
            selectedFont = fontName;
            break;
        }
    }

    titleFont.setFamily(selectedFont);
    titleFont.setPointSize(34);
    titleFont.setBold(true);
    titleFont.setItalic(true);

    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet(
        "color: white;"
        "background-color: rgba(0, 0, 0, 0);"
    );

    // 为标题添加阴影效果
    QGraphicsDropShadowEffect *titleShadow = new QGraphicsDropShadowEffect();
    titleShadow->setBlurRadius(15);
    titleShadow->setColor(QColor(0, 0, 0, 150));
    titleShadow->setOffset(3, 3);
    m_titleLabel->setGraphicsEffect(titleShadow);

    // 将标题放在左上角
    topLayout->addWidget(m_titleLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    topLayout->addStretch();

    m_mainLayout->addLayout(topLayout);

    // 4. 在标题和按钮之间添加弹性空间
    m_mainLayout->addStretch(11);

    // 5. 创建按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(35);
    buttonLayout->addStretch();

    // 5.1 开始游戏按钮
    m_startButton = new QPushButton("START", this);
    m_startButton->setFixedSize(150, 50);

    m_startButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255, 255, 255, 220);"
        "  color: #2c3e50;"
        "  border: 3px solid rgba(200, 200, 200, 200);"
        "  border-radius: 10px;"
        "  font-size: 24px;"
        "  font-weight: bold;"
        "  padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(240, 240, 240, 240);"
        "  border: 3px solid rgba(180, 180, 180, 240);"
        "  color: #2c3e50;"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(230, 230, 230, 250);"
        "  border: 3px solid rgba(160, 160, 160, 250);"
        "}"
    );

    QGraphicsDropShadowEffect *startShadow = new QGraphicsDropShadowEffect();
    startShadow->setBlurRadius(15);
    startShadow->setColor(QColor(0, 0, 0, 100));
    startShadow->setOffset(3, 3);
    m_startButton->setGraphicsEffect(startShadow);

    // 5.2 退出游戏按钮
    m_quitButton = new QPushButton("OFF", this);
    m_quitButton->setFixedSize(150, 50);

    m_quitButton->setStyleSheet(
        "QPushButton {"
        "  background-color: rgba(255, 255, 255, 220);"
        "  color: #2c3e50;"
        "  border: 3px solid rgba(200, 200, 200, 200);"
        "  border-radius: 10px;"
        "  font-size: 24px;"
        "  font-weight: bold;"
        "  padding: 10px 20px;"
        "}"
        "QPushButton:hover {"
        "  background-color: rgba(240, 240, 240, 240);"
        "  border: 3px solid rgba(180, 180, 180, 240);"
        "  color: #2c3e50;"
        "}"
        "QPushButton:pressed {"
        "  background-color: rgba(230, 230, 230, 250);"
        "  border: 3px solid rgba(160, 160, 160, 250);"
        "}"
    );

    QGraphicsDropShadowEffect *quitShadow = new QGraphicsDropShadowEffect();
    quitShadow->setBlurRadius(15);
    quitShadow->setColor(QColor(0, 0, 0, 100));
    quitShadow->setOffset(3, 3);
    m_quitButton->setGraphicsEffect(quitShadow);

    buttonLayout->addWidget(m_startButton);
    buttonLayout->addWidget(m_quitButton);
    buttonLayout->addStretch();

    m_mainLayout->addLayout(buttonLayout);

    // 6. 在按钮之间添加弹性空间
    buttonLayout->addStretch(5);
    buttonLayout->addWidget(m_startButton);
    buttonLayout->addStretch(2.5);
    buttonLayout->addWidget(m_quitButton);
    buttonLayout->addStretch(5);

    // 7. 在底部添加弹性空间
    m_mainLayout->addStretch(1);

    this->setLayout(m_mainLayout);

    // 8. 连接按钮信号
    connect(m_startButton, &QPushButton::clicked, this, &StartWindow::onStartButtonClicked);
    connect(m_quitButton, &QPushButton::clicked, this, &StartWindow::onQuitButtonClicked);

    qDebug() << "StartWindow 创建完成，标题和按钮位置已下调";
}
void StartWindow::onStartButtonClicked()
{
    qDebug() << "点击了【开始游戏】按钮";
    emit startButtonClicked();
}

void StartWindow::onQuitButtonClicked()
{
    qDebug() << "点击了【退出游戏】按钮";
    emit quitButtonClicked();
}

void StartWindow::loadBackgroundImage()
{
    QString bgPath = ":/new/prefix1/start1.png";

    if (QFile::exists(bgPath)) {
        if (m_backgroundPixmap.load(bgPath)) {
            m_backgroundPixmap = m_backgroundPixmap.scaled(
                this->size(),
                Qt::IgnoreAspectRatio,
                Qt::SmoothTransformation
            );
            qDebug() << "StartWindow 背景图加载成功:" << bgPath;
        } else {
            qWarning() << "无法加载 StartWindow 背景图:" << bgPath;
        }
    } else {
        qWarning() << "StartWindow 背景图文件不存在:" << bgPath;
    }
}

void StartWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    if (!m_backgroundPixmap.isNull()) {
        // 创建一个更透明的背景
        QPixmap transparentPixmap(m_backgroundPixmap.size());
        transparentPixmap.fill(Qt::transparent);

        QPainter pixmapPainter(&transparentPixmap);
        pixmapPainter.setOpacity(0.85);  // 70%透明度
        pixmapPainter.drawPixmap(0, 0, m_backgroundPixmap);
        pixmapPainter.end();

        painter.drawPixmap(0, 0, width(), height(), transparentPixmap);
    } else {
        // 备用背景色
        QLinearGradient gradient(0, 0, width(), height());
        gradient.setColorAt(0, QColor(30, 30, 50));
        gradient.setColorAt(1, QColor(20, 20, 40));
        painter.fillRect(rect(), gradient);
    }

    QDialog::paintEvent(event);
}

StartWindow::~StartWindow()
{
    // 自动清理
}
