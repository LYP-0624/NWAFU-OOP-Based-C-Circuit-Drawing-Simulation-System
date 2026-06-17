#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "resistoritem.h"
#include "capacitoritem.h"
#include "inductoritem.h"
#include "switchitem.h"
#include "grounditem.h"
#include "Circuit.h"
#include "Port.h"
#include "ComponentFactory.h"
#include "SimulationLogger.h"
#include "Resistor.h"
#include "PowerSource.h"
#include "Bulb.h"
#include "Ammeter.h"
#include "Voltmeter.h"
#include "Switch.h"
#include "Ground.h"
#include "Capacitor.h"
#include "Inductor.h"
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QDialog>
#include <QPushButton>
#include <QSet>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QDebug>
#include <QInputDialog>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QDialogButtonBox>
#include <QMouseEvent>
#include <memory>
#include <unordered_map>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_circuit(std::make_unique<CircuitSim::Circuit>())
{
    ui->setupUi(this);
    this->setWindowTitle("电路仿真工作台 v1.0");
    this->resize(1200, 800);

    QStatusBar *status = this->statusBar();
    status->showMessage("欢迎使用电路仿真系统！点击两个元件即可连线，Ctrl+S保存，Ctrl+O加载，空格运行仿真。");

    QToolBar *mainToolBar = new QToolBar("核心操作", this);
    mainToolBar->setMovable(false);
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);

    QAction *addResistorAction = new QAction("添加电阻", this);
    QAction *addPowerAction = new QAction("添加电源", this);
    QAction *addBulbAction = new QAction("添加灯泡", this);
    QAction *addAmmeterAction = new QAction("添加电流表", this);
    QAction *addVoltmeterAction = new QAction("添加电压表", this);
    QAction *addCapacitorAction = new QAction("添加电容", this);
    QAction *addInductorAction = new QAction("添加电感", this);
    QAction *addSwitchAction = new QAction("添加开关", this);
    QAction *addGroundAction = new QAction("接地", this);
    QAction *deleteAction = new QAction("删除选中项", this);
    QAction *solveAction = new QAction("运行仿真", this);

    mainToolBar->addAction(addResistorAction);
    mainToolBar->addAction(addPowerAction);
    mainToolBar->addAction(addBulbAction);
    mainToolBar->addAction(addAmmeterAction);
    mainToolBar->addAction(addVoltmeterAction);
    mainToolBar->addAction(addCapacitorAction);
    mainToolBar->addAction(addInductorAction);
    mainToolBar->addAction(addSwitchAction);
    mainToolBar->addAction(addGroundAction);
    mainToolBar->addSeparator();
    mainToolBar->addAction(deleteAction);
    mainToolBar->addAction(solveAction);

    createMenuBar();

    if (ui->circuitView) {
        ui->circuitView->setFocusPolicy(Qt::StrongFocus);
        ui->circuitView->setFocus();
    }

    m_scene = new QGraphicsScene(this);
    ui->circuitView->setScene(m_scene);
    m_scene->setSceneRect(-500, -400, 1000, 800);
    ui->circuitView->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
    ui->circuitView->setRenderHint(QPainter::Antialiasing);
    ui->circuitView->viewport()->installEventFilter(this);

    connect(addResistorAction, &QAction::triggered, this, [=]() { ui->btnResistor->click(); });
    connect(addPowerAction, &QAction::triggered, this, [=]() { ui->btnPower->click(); });
    connect(addBulbAction, &QAction::triggered, this, [=]() { ui->btnBulb->click(); });
    connect(addAmmeterAction, &QAction::triggered, this, [=]() { ui->btnAmmeter->click(); });
    connect(addVoltmeterAction, &QAction::triggered, this, [=]() { ui->btnVoltmeter->click(); });
    connect(addCapacitorAction, &QAction::triggered, this, [=]() { ui->capacitor->click(); });
    connect(addInductorAction, &QAction::triggered, this, [=]() { ui->inductor->click(); });
    connect(addSwitchAction, &QAction::triggered, this, [=]() { ui->switch1->click(); });
    connect(addGroundAction, &QAction::triggered, this, [=]() { ui->ground->click(); });
    connect(solveAction, &QAction::triggered, this, &MainWindow::onSolveCircuit);

    connect(ui->btnPower, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        PowerSourceItem *item = new PowerSourceItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::PowerSource* comp = new CircuitSim::PowerSource(m_componentCount, 0, 0, 5.0);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->btnResistor, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        ResistorItem *item = new ResistorItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Resistor* comp = new CircuitSim::Resistor(m_componentCount, 0, 0, 100.0);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->btnBulb, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        LightBulbItem *item = new LightBulbItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Bulb* comp = new CircuitSim::Bulb(m_componentCount, 0, 0, 10.0, 0.5);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->btnAmmeter, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        AmmeterItem *item = new AmmeterItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Ammeter* comp = new CircuitSim::Ammeter(m_componentCount, 0, 0);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->btnVoltmeter, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        VoltmeterItem *item = new VoltmeterItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Voltmeter* comp = new CircuitSim::Voltmeter(m_componentCount, 0, 0);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->ground, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        GroundItem *item = new GroundItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Ground* comp = new CircuitSim::Ground(m_componentCount, 0, 0);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->inductor, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        InductorItem *item = new InductorItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Inductor* comp = new CircuitSim::Inductor(m_componentCount, 0, 0, 1.0);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->capacitor, &QPushButton::clicked, this, [=]() {
        statusBar()->showMessage("成功添加电容元件！", 2000);
        m_componentCount++;
        CapacitorItem *item = new CapacitorItem(m_componentCount);
        item->setPos(50, 50);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Capacitor* comp = new CircuitSim::Capacitor(m_componentCount, 50, 50, 1e-6);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(ui->switch1, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        SwitchItem *item = new SwitchItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();

        CircuitSim::Switch* comp = new CircuitSim::Switch(m_componentCount, 0, 0, false);
        m_circuit->addComponent(comp);
        m_uiToBackendMap[item] = comp;
    });

    connect(deleteAction, &QAction::triggered, this, [=]() {
        if (!m_scene) return;
        QSet<QGraphicsItem*> itemsToDelete;
        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        for (QGraphicsItem* item : selectedItems) {
            itemsToDelete.insert(item);
            GraphComponent* comp = dynamic_cast<GraphComponent*>(item);
            if (comp) {
                auto it = m_uiToBackendMap.find(comp);
                if (it != m_uiToBackendMap.end()) {
                    m_circuit->removeComponent(it->second->getId());
                    m_uiToBackendMap.erase(it);
                }

                QList<QGraphicsItem*> allItems = m_scene->items();
                for (QGraphicsItem* sceneItem : allItems) {
                    WireItem* wire = dynamic_cast<WireItem*>(sceneItem);
                    if (wire && (wire->m_startItem == comp || wire->m_endItem == comp)) {
                        itemsToDelete.insert(wire);
                    }
                }
            }
        }
        for (QGraphicsItem* item : itemsToDelete) {
            if (item == m_firstClickedItem) m_firstClickedItem = nullptr;
            m_scene->removeItem(item);
            delete item;
        }
        m_scene->update();
    });

    connect(m_scene, &QGraphicsScene::selectionChanged, this, [=]() {
        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        if (selectedItems.size() == 1) {
            GraphComponent* clickedItem = dynamic_cast<GraphComponent*>(selectedItems.first());

            if (clickedItem) {
                if (m_firstClickedItem == nullptr) {
                    m_firstClickedItem = clickedItem;
                    m_firstClickedPort = clickedItem->getSelectedPort();
                } else if (m_firstClickedItem != clickedItem) {
                    GraphComponent* startItem = m_firstClickedItem;
                    int startPort = m_firstClickedPort;
                    int endPort = clickedItem->getSelectedPort();
                    WireItem *newWire = new WireItem(m_firstClickedItem, clickedItem, startPort, endPort);
                    m_scene->addItem(newWire);
                    m_scene->update();
                    m_firstClickedItem = nullptr;
                    m_scene->clearSelection();

                    auto it1 = m_uiToBackendMap.find(startItem);
                    auto it2 = m_uiToBackendMap.find(clickedItem);
                    if (it1 != m_uiToBackendMap.end() && it2 != m_uiToBackendMap.end()) {
                        CircuitSim::Component* comp1 = it1->second;
                        CircuitSim::Component* comp2 = it2->second;
                        if (comp1->getPorts().size() >= 2 && comp2->getPorts().size() >= 2) {
                            CircuitSim::Port* port1 = comp1->getPorts()[startPort];
                            CircuitSim::Port* port2 = comp2->getPorts()[endPort];

                            m_circuit->connect(port1, port2);
                        }
                    }
                }
            } else {
                m_firstClickedItem = nullptr;
            }
        } else {
            m_firstClickedItem = nullptr;
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenuBar()
{
    QMenu *fileMenu = menuBar()->addMenu("文件");
    QMenu *simMenu = menuBar()->addMenu("仿真");
    QMenu *helpMenu = menuBar()->addMenu("帮助");

    QAction *newAct = new QAction("新建", this);
    QAction *saveAct = new QAction("保存", this);
    QAction *loadAct = new QAction("加载", this);
    QAction *exitAct = new QAction("退出", this);

    QAction *solveAct = new QAction("运行仿真", this);
    QAction *logAct = new QAction("查看日志", this);

    QAction *aboutAct = new QAction("关于", this);

    fileMenu->addAction(newAct);
    fileMenu->addAction(saveAct);
    fileMenu->addAction(loadAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    simMenu->addAction(solveAct);
    simMenu->addAction(logAct);

    helpMenu->addAction(aboutAct);

    newAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_N));
    saveAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    loadAct->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));

    connect(newAct, &QAction::triggered, this, &MainWindow::onNewCircuit);
    connect(saveAct, &QAction::triggered, this, &MainWindow::onSaveCircuit);
    connect(loadAct, &QAction::triggered, this, &MainWindow::onLoadCircuit);
    connect(exitAct, &QAction::triggered, this, &QMainWindow::close);

    connect(solveAct, &QAction::triggered, this, &MainWindow::onSolveCircuit);
    connect(logAct, &QAction::triggered, this, &MainWindow::onShowLog);

    connect(aboutAct, &QAction::triggered, this, []() {
        QMessageBox::about(nullptr, "关于", "电路仿真系统 v1.0\n基于Qt和面向对象设计");
    });
}

void MainWindow::syncUIToBackend()
{
    for (auto& [uiItem, backendComp] : m_uiToBackendMap) {
        QPointF pos = uiItem->scenePos();
        backendComp->setPosition(pos.x(), pos.y());
        backendComp->setName(backendComp->getTypeName() + "_" + std::to_string(backendComp->getId()));

        SwitchItem* swItem = dynamic_cast<SwitchItem*>(uiItem);
        if (swItem && backendComp->getType() == CircuitSim::ComponentType::SWITCH) {
            CircuitSim::Switch* swComp = dynamic_cast<CircuitSim::Switch*>(backendComp);
            if (swComp) {
                swComp->setState(swItem->m_isClosed);
            }
        }
    }
}

void MainWindow::onNewCircuit()
{
    m_scene->clear();
    m_circuit->clear();
    m_uiToBackendMap.clear();
    m_componentCount = 0;
    m_firstClickedItem = nullptr;
    m_isPowered = false;
    this->setWindowTitle("电路仿真工作台 v1.0 - 新建电路");
    statusBar()->showMessage("已新建空白电路");
}

void MainWindow::onSaveCircuit()
{
    syncUIToBackend();
    QString filePath = QFileDialog::getSaveFileName(this, "保存电路", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        if (m_circuit->saveToJsonFile(filePath.toStdString())) {
            statusBar()->showMessage("电路保存成功: " + filePath);
        } else {
            QMessageBox::warning(this, "保存失败", "无法保存电路文件");
        }
    }
}

void MainWindow::onLoadCircuit()
{
    QString filePath = QFileDialog::getOpenFileName(this, "加载电路", "", "JSON Files (*.json)");
    if (!filePath.isEmpty()) {
        m_scene->clear();
        m_uiToBackendMap.clear();
        m_firstClickedItem = nullptr;

        CircuitSim::Circuit* loadedCircuit = new CircuitSim::Circuit();
        if (loadedCircuit->loadFromJsonFile(filePath.toStdString())) {
            m_circuit.reset(loadedCircuit);
            m_componentCount = 0;

            for (const auto& pair : m_circuit->getComponents()) {
                const auto& comp = pair.second;
                int id = comp->getId();
                GraphComponent* item = nullptr;
                CircuitSim::ComponentType type = comp->getType();

                if (type == CircuitSim::ComponentType::RESISTOR) item = new ResistorItem(id);
                else if (type == CircuitSim::ComponentType::POWER_SOURCE) item = new PowerSourceItem(id);
                else if (type == CircuitSim::ComponentType::BULB) item = new LightBulbItem(id);
                else if (type == CircuitSim::ComponentType::AMMETER) item = new AmmeterItem(id);
                else if (type == CircuitSim::ComponentType::VOLTMETER) item = new VoltmeterItem(id);
                else if (type == CircuitSim::ComponentType::CAPACITOR) item = new CapacitorItem(id);
                else if (type == CircuitSim::ComponentType::INDUCTOR) item = new InductorItem(id);
                else if (type == CircuitSim::ComponentType::SWITCH) item = new SwitchItem(id);
                else if (type == CircuitSim::ComponentType::GROUND) item = new GroundItem(id);

                if (item) {
                    item->setPos(comp->getX(), comp->getY());
                    m_scene->addItem(item);
                    m_uiToBackendMap[item] = comp;
                    if (id >= m_componentCount) m_componentCount = id + 1;

                    if (type == CircuitSim::ComponentType::SWITCH) {
                        CircuitSim::Switch* swComp = dynamic_cast<CircuitSim::Switch*>(comp);
                        SwitchItem* swItem = dynamic_cast<SwitchItem*>(item);
                        if (swComp && swItem) {
                            swItem->m_isClosed = swComp->isOn();
                            swItem->update();
                        }
                    }
                }
            }

            statusBar()->showMessage("电路加载成功: " + filePath);
        } else {
            delete loadedCircuit;
            QMessageBox::warning(this, "加载失败", "无法加载电路文件：文件格式错误或损坏");
        }
    }
}

void MainWindow::onSolveCircuit()
{
    syncUIToBackend();
    m_isPowered = true;
    this->setWindowTitle("电路仿真工作台 v1.0 - 【仿真计算中】");

    statusBar()->showMessage("正在检查电路...");

    int componentCount = static_cast<int>(m_circuit->getComponents().size());
    int nodeCount = static_cast<int>(m_circuit->getNodes().size());
    int branchCount = static_cast<int>(m_circuit->getBranches().size());

    QString debugMsg = QString("元件数: %1, 节点数: %2, 支路数: %3").arg(componentCount).arg(nodeCount).arg(branchCount);
    qDebug() << debugMsg;
    statusBar()->showMessage(debugMsg);

    if (m_circuit->solve()) {
        updateSimulationResults();
        statusBar()->showMessage("仿真计算成功！");
        m_circuit->logSimulation("电路仿真计算完成");
        qDebug() << "=== 仿真结果 ===";
        for (const auto& result : m_circuit->getResults()) {
            qDebug() << QString::fromStdString(result.componentName)
                      << "V=" << result.voltage << "V, I=" << result.current << "A";
        }
    } else {
        QMessageBox::warning(this, "仿真失败",
            QString("电路求解失败！\n元件数: %1\n节点数: %2\n支路数: %3\n\n请检查：\n1. 是否添加了电源\n2. 电路是否形成闭合回路\n3. 所有元件是否正确连接")
                .arg(componentCount).arg(nodeCount).arg(branchCount));
    }
}

void MainWindow::onShowLog()
{
    QDialog *logDialog = new QDialog(this);
    logDialog->setWindowTitle("仿真日志");
    logDialog->resize(800, 600);

    QTextEdit *logText = new QTextEdit(logDialog);
    logText->setReadOnly(true);
    logText->setPlainText(QString::fromStdString(CircuitSim::SimulationLogger::getInstance().getLog()));

    QVBoxLayout *layout = new QVBoxLayout(logDialog);
    layout->addWidget(logText);
    logDialog->setLayout(layout);
    logDialog->show();
}

void MainWindow::updateSimulationResults()
{
    auto results = m_circuit->getResults();
    QList<QGraphicsItem*> allItems = m_scene->items();

    for (const auto& result : results) {
        for (QGraphicsItem* item : allItems) {
            GraphComponent* comp = dynamic_cast<GraphComponent*>(item);
            if (comp && comp->getId() == result.componentId) {
                comp->updatePhysicsData(result.voltage, result.current);

                SwitchItem* swItem = dynamic_cast<SwitchItem*>(item);
                if (swItem && result.extra.count("state")) {
                    swItem->m_isClosed = (result.extra.at("state") > 0.5);
                    swItem->update();
                }

                LightBulbItem* bulbItem = dynamic_cast<LightBulbItem*>(item);
                if (bulbItem && result.extra.count("isLit")) {
                    bool lit = (result.extra.at("isLit") > 0.5);
                    bulbItem->m_isLit = lit;
                    bulbItem->updatePhysicsData(result.voltage, result.current);
                }

                break;
            }
        }
    }

    m_scene->update();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        m_isPowered = !m_isPowered;

        if (m_isPowered) {
            syncUIToBackend();
            this->setWindowTitle("电路仿真工作台 v1.0 - 【通电仿真中】");
            if (m_circuit->solve()) {
                updateSimulationResults();
                m_circuit->logSimulation("电路仿真计算完成（空格键触发）");
                statusBar()->showMessage("仿真计算成功！");
            } else {
                QMessageBox::warning(this, "仿真失败", "电路求解失败，请检查电路连接");
                m_isPowered = false;
            }
        } else {
            this->setWindowTitle("电路仿真工作台 v1.0 - 【断开】");
            QList<QGraphicsItem*> allItems = m_scene->items();
            for (QGraphicsItem* item : allItems) {
                GraphComponent* component = dynamic_cast<GraphComponent*>(item);
                if (component) {
                    component->updatePhysicsData(0.0, 0.0);
                }
            }
            m_scene->update();
        }
        event->accept();
        return;
    } else if (event->key() == Qt::Key_1) {
        if (!m_scene) return;

        QSet<QGraphicsItem*> itemsToDelete;
        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        for (QGraphicsItem* item : selectedItems) {
            itemsToDelete.insert(item);
            GraphComponent* comp = dynamic_cast<GraphComponent*>(item);
            if (comp) {
                auto it = m_uiToBackendMap.find(comp);
                if (it != m_uiToBackendMap.end()) {
                    m_circuit->removeComponent(it->second->getId());
                    m_uiToBackendMap.erase(it);
                }

                QList<QGraphicsItem*> allItems = m_scene->items();
                for (QGraphicsItem* sceneItem : allItems) {
                    WireItem* wire = dynamic_cast<WireItem*>(sceneItem);
                    if (wire && (wire->m_startItem == comp || wire->m_endItem == comp)) {
                        itemsToDelete.insert(wire);
                    }
                }
            }
        }

        for (QGraphicsItem* item : itemsToDelete) {
            if (item == m_firstClickedItem) {
                m_firstClickedItem = nullptr;
            }
            m_scene->removeItem(item);
            delete item;
        }

        m_scene->update();
        event->accept();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QPointF scenePos = ui->circuitView->mapToScene(mouseEvent->pos());
        QGraphicsItem *item = m_scene->itemAt(scenePos, QTransform());
        GraphComponent *comp = dynamic_cast<GraphComponent*>(item);
        if (comp) {
            showEditDialog(comp);
            return true;
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::showEditDialog(GraphComponent* item)
{
    if (!item) return;

    auto it = m_uiToBackendMap.find(item);
    if (it == m_uiToBackendMap.end()) return;

    CircuitSim::Component* backendComp = it->second;
    CircuitSim::ComponentType type = backendComp->getType();

    QDialog dialog(this);
    dialog.setWindowTitle("编辑元件参数");

    QFormLayout *form = new QFormLayout(&dialog);
    QDoubleSpinBox *spin1 = nullptr;
    QDoubleSpinBox *spin2 = nullptr;
    QString label1, label2;

    switch (type) {
        case CircuitSim::ComponentType::RESISTOR: {
            label1 = "电阻值 (Ω)";
            CircuitSim::Resistor* r = dynamic_cast<CircuitSim::Resistor*>(backendComp);
            double val = r ? r->getResistance() : 100.0;
            spin1 = new QDoubleSpinBox(&dialog);
            spin1->setRange(0.001, 1e9);
            spin1->setDecimals(3);
            spin1->setValue(val);
            break;
        }
        case CircuitSim::ComponentType::POWER_SOURCE: {
            label1 = "电压值 (V)";
            CircuitSim::PowerSource* ps = dynamic_cast<CircuitSim::PowerSource*>(backendComp);
            double val = ps ? ps->getValue() : 5.0;
            spin1 = new QDoubleSpinBox(&dialog);
            spin1->setRange(-1000, 1000);
            spin1->setDecimals(3);
            spin1->setValue(val);
            break;
        }
        case CircuitSim::ComponentType::BULB: {
            label1 = "电阻值 (Ω)";
            label2 = "额定功率 (W)";
            CircuitSim::Bulb* b = dynamic_cast<CircuitSim::Bulb*>(backendComp);
            double r = b ? b->getResistance() : 10.0;
            spin1 = new QDoubleSpinBox(&dialog);
            spin1->setRange(0.1, 1e6);
            spin1->setDecimals(3);
            spin1->setValue(r);
            spin2 = new QDoubleSpinBox(&dialog);
            spin2->setRange(0.01, 1000);
            spin2->setDecimals(3);
            spin2->setValue(0.5);
            break;
        }
        case CircuitSim::ComponentType::CAPACITOR: {
            label1 = "电容值 (F)";
            CircuitSim::Capacitor* c = dynamic_cast<CircuitSim::Capacitor*>(backendComp);
            double val = c ? c->getCapacitance() : 1e-6;
            spin1 = new QDoubleSpinBox(&dialog);
            spin1->setRange(1e-12, 1.0);
            spin1->setDecimals(12);
            spin1->setValue(val);
            break;
        }
        case CircuitSim::ComponentType::INDUCTOR: {
            label1 = "电感值 (H)";
            CircuitSim::Inductor* ind = dynamic_cast<CircuitSim::Inductor*>(backendComp);
            double val = ind ? ind->getInductance() : 1.0;
            spin1 = new QDoubleSpinBox(&dialog);
            spin1->setRange(1e-9, 1000);
            spin1->setDecimals(9);
            spin1->setValue(val);
            break;
        }
        default:
            return;
    }

    if (spin1) {
        form->addRow(label1, spin1);
    }
    if (spin2 && !label2.isEmpty()) {
        form->addRow(label2, spin2);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    form->addRow(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        switch (type) {
            case CircuitSim::ComponentType::RESISTOR: {
                CircuitSim::Resistor* r = dynamic_cast<CircuitSim::Resistor*>(backendComp);
                if (r && spin1) r->setResistance(spin1->value());
                break;
            }
            case CircuitSim::ComponentType::POWER_SOURCE: {
                CircuitSim::PowerSource* ps = dynamic_cast<CircuitSim::PowerSource*>(backendComp);
                if (ps && spin1) ps->setValue(spin1->value());
                break;
            }
            case CircuitSim::ComponentType::BULB: {
                CircuitSim::Bulb* b = dynamic_cast<CircuitSim::Bulb*>(backendComp);
                if (b && spin1) b->setResistance(spin1->value());
                break;
            }
            case CircuitSim::ComponentType::CAPACITOR: {
                CircuitSim::Capacitor* c = dynamic_cast<CircuitSim::Capacitor*>(backendComp);
                if (c && spin1) c->setCapacitance(spin1->value());
                break;
            }
            case CircuitSim::ComponentType::INDUCTOR: {
                CircuitSim::Inductor* ind = dynamic_cast<CircuitSim::Inductor*>(backendComp);
                if (ind && spin1) ind->setInductance(spin1->value());
                break;
            }
            default:
                break;
        }
        statusBar()->showMessage("参数已更新", 2000);
    }
}