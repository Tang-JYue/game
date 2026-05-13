// main.cpp
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <QDateTime>
#include "gamewindow.h"

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

    GameWindow window;
    window.show();

    int result = app.exec();

    qDebug() << "===== 程序退出 =====";
    // 恢复默认消息处理
    qInstallMessageHandler(0);
    return result;
}
