#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "resistoritem.h" // 👈 核心关键：直接在这里把物资库死死焊住
// 👇 必须在这里包含新元件的头文件！(前提是你已经新建了这些类)
#include "capacitoritem.h"
#include "inductoritem.h"
#include "switchitem.h"
#include "grounditem.h"

#include <QKeyEvent>
#include <QSet>
#include <QToolBar>
#include <QStatusBar>
#include <QAction>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("电路仿真工作台 v1.0 - 实习专属版");
    this->resize(1000, 700);

    // 2. 🌟 创建底部状态栏（给用户实时反馈）
    QStatusBar *status = this->statusBar();
    status->showMessage("欢迎使用电路仿真系统！提示：点击元件后按 '1' 删除，或使用上方工具栏。");

    // 3. 🌟 创建顶部可视化工具栏
    QToolBar *mainToolBar = new QToolBar("核心操作", this);
    mainToolBar->setMovable(false); // 固定在顶部，防止被意外拖走
    this->addToolBar(Qt::TopToolBarArea, mainToolBar);

    // 4. 🌟 添加工具栏按钮（带 Emoji 图标，直观好看）
    QAction *addResistorAction = new QAction("➕ 添加电阻", this);
    QAction *addPowerAction = new QAction("🔋 添加电源", this);

    // 👇 新增四大金刚按钮
    QAction *addCapacitorAction = new QAction("⏸️ 添加电容", this);
    QAction *addInductorAction = new QAction("〰️ 添加电感", this);
    QAction *addSwitchAction = new QAction("🎚️ 添加开关", this);
    QAction *addGroundAction = new QAction("⏚ 接地", this);

    QAction *deleteAction = new QAction("🗑️ 删除选中项", this);

    // 将按钮塞进工具栏
    mainToolBar->addAction(addResistorAction);
    mainToolBar->addAction(addPowerAction);
    mainToolBar->addAction(addCapacitorAction);
    mainToolBar->addAction(addInductorAction);
    mainToolBar->addAction(addSwitchAction);
    mainToolBar->addAction(addGroundAction);
    mainToolBar->addSeparator(); // 添加一条竖线分隔符，让UI更有层次
    mainToolBar->addAction(deleteAction);

    if (ui->circuitView) {
        ui->circuitView->setFocusPolicy(Qt::StrongFocus); // 赋予最高键盘焦点支配权
        ui->circuitView->setFocus();                      // 启动时默认就把焦点死死锁在画布上
    }

    // 1. 初始化舞台并将其塞入画布中
    m_scene = new QGraphicsScene(this);
    ui->circuitView->setScene(m_scene);
    m_scene->setSceneRect(-400, -300, 800, 600);

    // =================================================================
    // 💡 工具栏按钮事件绑定 (由于原来有 ui->btnPower 等，这里顺便把工具栏按钮也绑上)
    // =================================================================
    connect(addResistorAction, &QAction::triggered, this, [=]() { ui->btnResistor->click(); });
    connect(addPowerAction, &QAction::triggered, this, [=]() { ui->btnPower->click(); });

    // 💡 连招1：点击电源按钮
    connect(ui->btnPower, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        PowerSourceItem *item = new PowerSourceItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr; // 撕掉连线小本本，清空记忆
        m_scene->clearSelection();
    });

    // 💡 连招2：点击电阻按钮
    connect(ui->btnResistor, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        ResistorItem *item = new ResistorItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });

    // 💡 连招3：点击灯泡按钮
    connect(ui->btnBulb, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        LightBulbItem *item = new LightBulbItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });

    // 💡 连招4：点击电流表按钮
    connect(ui->btnAmmeter, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        AmmeterItem *item = new AmmeterItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });

    // 💡 连招5：点击电压表按钮
    connect(ui->btnVoltmeter, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        VoltmeterItem *item = new VoltmeterItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });

    // 💡 连招：点击【接地】按钮
    connect(ui->ground, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        GroundItem *item = new GroundItem(m_componentCount);
        item->setPos(0, 0); // 降落在画布正中心
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });

    // 💡 连招：点击【电感】按钮
    connect(ui->inductor, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        InductorItem *item = new InductorItem(m_componentCount);
        item->setPos(0, 0); // 降落在画布正中心
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });
    // 💡 连招：点击【电容】按钮
    connect(ui->capacitor, &QPushButton::clicked, this, [=]() {
        // 在底部状态栏打字确认
        this->statusBar()->showMessage("⏸️ 成功呼叫电容元件！", 2000);

        m_componentCount++;
        CapacitorItem *item = new CapacitorItem(m_componentCount);

        // 同样让它生成在 (50, 50) 的位置，防止和中心点叠罗汉
        item->setPos(50, 50);

        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });

    // 💡 连招：点击【开关】按钮
    connect(ui->switch1, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        SwitchItem *item = new SwitchItem(m_componentCount);
        item->setPos(0, 0); // 降落在画布正中心
        m_scene->addItem(item);
        m_firstClickedItem = nullptr;
        m_scene->clearSelection();
    });

    // 👇 连招10：让工具栏的【删除选中项】按钮生效 (复用数字1的逻辑)
    connect(deleteAction, &QAction::triggered, this, [=]() {
        if (!m_scene) return;
        QSet<QGraphicsItem*> itemsToDelete;
        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        for (QGraphicsItem* item : selectedItems) {
            itemsToDelete.insert(item);
            GraphComponent* comp = dynamic_cast<GraphComponent*>(item);
            if (comp) {
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

    // =================================================================
    // 🌟 第三关总攻：智能拓扑连线控制逻辑
    // =================================================================
    connect(m_scene, &QGraphicsScene::selectionChanged, this, [=]() {
        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        if (selectedItems.size() == 1) {
            GraphComponent* clickedItem = dynamic_cast<GraphComponent*>(selectedItems.first());

            if (clickedItem) {
                if (m_firstClickedItem == nullptr) {
                    m_firstClickedItem = clickedItem;
                }
                else if (m_firstClickedItem != clickedItem) {
                    WireItem *newWire = new WireItem(m_firstClickedItem, clickedItem);
                    m_scene->addItem(newWire);
                    m_scene->update();

                    m_firstClickedItem = nullptr;
                    m_scene->clearSelection();
                }
            }
        }
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// =================================================================
// 🌟 终极进化：键盘全面接管！【空格】控电闸，数字【1】无情抹除元件/导线
// =================================================================
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 🔥 【空格键】控制通电与断电：注入物理灵魂
    if (event->key() == Qt::Key_Space) {
        m_isPowered = !m_isPowered; // 电闸状态反转

        if (m_isPowered) {
            this->setWindowTitle("简易电路仿真系统 —— 【⚡ 闭合通电中 ⚡】");
        } else {
            this->setWindowTitle("简易电路仿真系统 —— 【❌ 断开闭合 ❌】");
        }

        // 遍历大舞台，强行灌入模拟的物理数据
        QList<QGraphicsItem*> allItems = m_scene->items();
        for (QGraphicsItem* item : allItems) {
            GraphComponent* component = dynamic_cast<GraphComponent*>(item);
            if (component) {
                if (m_isPowered) {
                    component->updatePhysicsData(5.0, 2.0); // 通电：模拟 5V 电压，2A 电流
                } else {
                    component->updatePhysicsData(0.0, 0.0); // 断电：数据全部归零
                }
            }
        }
        m_scene->update(); // 强制刷新大舞台，让灯泡变色、电表数字跳动！
        event->accept();
        return;
    }

    // 🔥 【数字键 1】：无情且安全地单独抹除导线，或抹除元件及其关联导线
    else if (event->key() == Qt::Key_1) { // 👈 已经完美改为了键盘上的数字 1
        if (!m_scene) return;

        QSet<QGraphicsItem*> itemsToDelete;
        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        for (QGraphicsItem* item : selectedItems) {
            itemsToDelete.insert(item); // 把选中的东西（不管是元件还是导线）直接加入死亡名单

            // 尝试看看它是不是一个核心元件，如果是，株连九族删导线
            GraphComponent* comp = dynamic_cast<GraphComponent*>(item);
            if (comp) {
                QList<QGraphicsItem*> allItems = m_scene->items();
                for (QGraphicsItem* sceneItem : allItems) {
                    WireItem* wire = dynamic_cast<WireItem*>(sceneItem);
                    if (wire && (wire->m_startItem == comp || wire->m_endItem == comp)) {
                        itemsToDelete.insert(wire);
                    }
                }
            }
        }

        // 统一执行安全死刑
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