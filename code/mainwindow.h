#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene> // 👈 必须在这里显式引入舞台，否则编译器不认识 QGraphicsScene 类型的指针
#include "graphcomponent.h"
#include <QKeyEvent>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    // 🌟 核心追加：重写键盘按下事件，接管系统“总电闸”
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::MainWindow *ui;
    QGraphicsScene *m_scene = nullptr;
    int m_componentCount = 0;
    GraphComponent* m_firstClickedItem = nullptr;

    bool m_isPowered = false; // 🌟 核心追加：记录当前整个电路的通断电状态（false=断电，true=通电）
};
#endif // MAINWINDOW_H