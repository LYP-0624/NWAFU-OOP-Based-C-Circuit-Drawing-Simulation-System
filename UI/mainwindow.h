#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QKeyEvent>
#include <QMainWindow>
#include <memory>

#include "Circuit.h"
#include "circuitscene.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

// 主窗口
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onNewCircuit();      // 新建电路
    void onSaveCircuit();     // 保存电路
    void onLoadCircuit();     // 加载电路
    void onSolveCircuit();    // 执行仿真
    void onShowHelp();        // 显示帮助
    void onAbout();           // 关于对话框
    void onClearAll();        // 清空画布

    void onAddResistor();     // 添加电阻
    void onAddPowerSource();  // 添加电源
    void onAddBulb();         // 添加灯泡
    void onAddAmmeter();      // 添加电流表
    void onAddVoltmeter();    // 添加电压表
    void onAddCapacitor();    // 添加电容
    void onAddInductor();     // 添加电感
    void onAddSwitch();       // 添加开关

private:
    Ui::MainWindow* ui;
    CircuitScene* m_scene;
    std::unique_ptr<CircuitSim::Circuit> m_circuit;

    void setupMenuBar();             // 初始化菜单栏
    void setupToolBar();             // 初始化工具栏
    void setupConnections();         // 建立信号槽连接
    void updateStatusForTool(const QString& text); // 更新状态栏
    void performSimulation();        // 执行仿真流程
    void resetCircuit();             // 重置电路
};

#endif // MAINWINDOW_H
