#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "gamewindow.h"
#include "startwindow.h"
#include "rulewindow.h"

// 自定义消息处理函数，将日志输出到文件和控制台
void myMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    Q_UNUSED(context);

    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("[Debug] %1").arg(msg);
        break;
    // ... 可以处理其他类型如 Warning, Critical, Fatal
    default:
        break;
    }

    // 输出到控制台（如果可能）
    QTextStream out(stdout);
    out << txt << endl;

    // 输出到文件
    QFile outFile("game_debug.log");
    if (outFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        QTextStream textStream(&outFile);
        textStream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz") << ": " << txt << endl;
        outFile.close();
    }
}

int main(int argc, char* argv[])
{
    // 安装自定义消息处理函数，捕获所有qDebug
    qInstallMessageHandler(myMessageHandler);

    QApplication app(argc, argv);
    QFont font("SimHei", 9);
    app.setFont(font);

    qDebug() << "===== 程序启动 (日志已重定向到文件) =====";

    // 1. 创建游戏窗口（但不显示）
    GameWindow *gameWindow = new GameWindow();
    gameWindow->hide();  // 先隐藏

    // 2. 创建规则窗口（但不显示）
    RuleWindow *ruleWindow = new RuleWindow();
    ruleWindow->hide();  // 先隐藏

    // 3. 创建启动窗口（首先显示）
    StartWindow *startWindow = new StartWindow();

    // 4. 连接信号槽，控制窗口切换流程

    // 4.1 启动窗口 -> 规则窗口
    QObject::connect(startWindow, &StartWindow::startButtonClicked, [startWindow, ruleWindow]() {
        qDebug() << "从启动窗口切换到规则窗口";
        startWindow->hide();
        ruleWindow->show();
    });

    // 4.2 启动窗口 -> 退出程序
    QObject::connect(startWindow, &StartWindow::quitButtonClicked, [startWindow, &app]() {
        qDebug() << "用户选择退出程序";
        startWindow->close();
        app.quit();
    });

    // 4.3 规则窗口 -> 游戏窗口
    QObject::connect(ruleWindow, &RuleWindow::continueButtonClicked, [ruleWindow, gameWindow]() {
        qDebug() << "从规则窗口切换到游戏窗口";
        ruleWindow->hide();
        gameWindow->show();
        gameWindow->setFocus();  // 确保游戏窗口获得焦点
    });

    // 4.4 处理游戏窗口关闭事件
    QObject::connect(gameWindow, &GameWindow::destroyed, [&app]() {
        qDebug() << "游戏窗口关闭，退出程序";
        app.quit();
    });

    // 5. 显示启动窗口
    startWindow->show();

    int result = app.exec();

    qDebug() << "===== 程序退出 =====";

    // 清理内存
    delete startWindow;
    delete ruleWindow;
    delete gameWindow;

    // 恢复默认消息处理
    qInstallMessageHandler(0);
    return result;
}
