#include "mainwindow.h"

#include <QApplication>
#include <QFont>
#include <QLocale>

// 程序入口函数
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);  // 创建Qt应用程序对象
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));  // 设置默认语言环境为中文（中国）
    a.setFont(QFont(QStringLiteral("Microsoft YaHei"), 10));  // 设置全局字体为微软雅黑，字号10
    MainWindow w;  // 创建主窗口
    w.show();      // 显示主窗口
    return a.exec();  // 进入Qt事件循环
}