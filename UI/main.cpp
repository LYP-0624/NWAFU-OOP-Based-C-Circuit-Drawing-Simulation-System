#include "mainwindow.h"

#include <QApplication>
#include <QFont>
#include <QLocale>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    QLocale::setDefault(QLocale(QLocale::Chinese, QLocale::China));
    a.setFont(QFont(QStringLiteral("Microsoft YaHei"), 10));
    MainWindow w;
    w.show();
    return a.exec();
}
