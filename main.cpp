#include <QApplication>
#include <QDebug>

#include "gamewindow.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    // 设置中文字体
    QFont font("SimHei", 9);
    app.setFont(font);

    qDebug() << "===== 程序启动 =====";

    GameWindow window;
    window.show();

    int result = app.exec();

    qDebug() << "===== 程序退出 =====";

    return result;
}
