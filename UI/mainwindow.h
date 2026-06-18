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

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

protected:
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    void onNewCircuit();
    void onSaveCircuit();
    void onLoadCircuit();
    void onSolveCircuit();
    void onShowHelp();
    void onAbout();
    void onClearAll();

    void onAddResistor();
    void onAddPowerSource();
    void onAddBulb();
    void onAddAmmeter();
    void onAddVoltmeter();
    void onAddCapacitor();
    void onAddInductor();
    void onAddSwitch();

private:
    Ui::MainWindow* ui;
    CircuitScene* m_scene;
    std::unique_ptr<CircuitSim::Circuit> m_circuit;

    void setupMenuBar();
    void setupToolBar();
    void setupConnections();
    void updateStatusForTool(const QString& text);
    void performSimulation();
    void resetCircuit();
};

#endif // MAINWINDOW_H
