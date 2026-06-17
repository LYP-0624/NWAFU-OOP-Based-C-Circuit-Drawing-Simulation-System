#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QKeySequence>
#include <QMenuBar>
#include <memory>
#include <unordered_map>
#include "graphcomponent.h"
#include "Circuit.h"

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
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onNewCircuit();
    void onSaveCircuit();
    void onLoadCircuit();
    void onSolveCircuit();
    void onShowLog();

private:
    Ui::MainWindow *ui;
    QGraphicsScene *m_scene = nullptr;
    int m_componentCount = 0;
    GraphComponent* m_firstClickedItem = nullptr;
    int m_firstClickedPort = 0;
    bool m_isPowered = false;
    
    std::unique_ptr<CircuitSim::Circuit> m_circuit;
    std::unordered_map<GraphComponent*, CircuitSim::Component*> m_uiToBackendMap;
    
    void createMenuBar();
    void syncUIToBackend();
    void updateSimulationResults();
    void showEditDialog(GraphComponent* item);
};
#endif // MAINWINDOW_H