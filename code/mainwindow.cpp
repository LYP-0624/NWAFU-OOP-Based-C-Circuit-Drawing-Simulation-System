#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "resistoritem.h" // 👈 核心关键：直接在这里把物资库死死焊住，双保险，彻底断绝“未定义标识符”报错！
#include <QKeyEvent>
#include <QSet>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    if (ui->circuitView) {
        ui->circuitView->setFocusPolicy(Qt::StrongFocus); // 赋予最高键盘焦点支配权
        ui->circuitView->setFocus();                      // 启动时默认就把焦点死死锁在画布上
    }

    // 1. 初始化舞台并将其塞入画布中
    m_scene = new QGraphicsScene(this);
    ui->circuitView->setScene(m_scene);
    m_scene->setSceneRect(-400, -300, 800, 600);

    // 💡 连招1：点击电源按钮
    connect(ui->btnPower, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        PowerSourceItem *item = new PowerSourceItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
    });

    // 💡 连招2：点击电阻按钮
    connect(ui->btnResistor, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        ResistorItem *item = new ResistorItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
    });

    // 💡 连招3：点击灯泡按钮
    connect(ui->btnBulb, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        LightBulbItem *item = new LightBulbItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
    });

    // 💡 连招4：点击电流表按钮
    connect(ui->btnAmmeter, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        AmmeterItem *item = new AmmeterItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
    });

    // 💡 连招5：点击电压表按钮
    connect(ui->btnVoltmeter, &QPushButton::clicked, this, [=]() {
        m_componentCount++;
        VoltmeterItem *item = new VoltmeterItem(m_componentCount);
        item->setPos(0, 0);
        m_scene->addItem(item);
    });
    // =================================================================
    // 🌟 第三关总攻：智能拓扑连线控制逻辑
    // =================================================================
    // 监听大舞台上任何元器件被点击的事件！
    connect(m_scene, &QGraphicsScene::selectionChanged, this, [=]() {
        // 获取当前整个舞台上被鼠标点中选中的所有元件
        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        // 如果用户精准点中了一个电路元件
        if (selectedItems.size() == 1) {
            GraphComponent* clickedItem = dynamic_cast<GraphComponent*>(selectedItems.first());

            if (clickedItem) {
                // 如果这是用户点的【第一个】元件，先把它默默记在小本本上
                if (m_firstClickedItem == nullptr) {
                    m_firstClickedItem = clickedItem;
                }
                // 如果用户之前已经点过一个了，这代表点的是【第二个】元件！连线触发！
                else if (m_firstClickedItem != clickedItem) {

                    // 直接传入第一次点击的元件和第二次点击的元件
                    WireItem *newWire = new WireItem(m_firstClickedItem, clickedItem);


                    // 啪！把导线当场空投进大舞台
                    m_scene->addItem(newWire);

                    // 连线成功后，强行让导线所在的包络圈刷新重绘一次，防止视觉死角
                    m_scene->update();

                    // 状态机功成身退，清空指针，等待下一次电路大连线
                    m_firstClickedItem = nullptr;
                    m_scene->clearSelection(); // 取消元件的高亮选中状态
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
// 🌟 终极进化：键盘全面接管！【空格】控电闸，【Delete】无情抹除元件
// =================================================================
void MainWindow::keyPressEvent(QKeyEvent *event)
{
    // 【空格键】控制通电与断电 (保持你原来的代码不动)
    if (event->key() == Qt::Key_Space) {
        // ... (你之前写的通电代码) ...
        event->accept();
        return;
    }

    // 🔥【Delete 键】或【Backspace 键】：无情且安全地抹除元件和关联导线
    else if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {

        if (!m_scene) return;

        // 1. 建立一个“死亡名单”（QSet 保证同一个东西不会被重复加入，防止二次释放闪退）
        QSet<QGraphicsItem*> itemsToDelete;

        QList<QGraphicsItem*> selectedItems = m_scene->selectedItems();

        // 2. 遍历选中的元件，把它们，以及连在它们身上的导线，通通拉入死亡名单
        for (QGraphicsItem* item : selectedItems) {
            itemsToDelete.insert(item); // 把选中的元件本身加入名单

            // 尝试看看它是不是一个核心元件
            GraphComponent* comp = dynamic_cast<GraphComponent*>(item);
            if (comp) {
                // 🚨 如果是核心元件，扫荡全场，把连在它身上的导线全部找出来！
                QList<QGraphicsItem*> allItems = m_scene->items();
                for (QGraphicsItem* sceneItem : allItems) {
                    WireItem* wire = dynamic_cast<WireItem*>(sceneItem);
                    if (wire) {
                        // 注意：这里的 m_startItem 和 m_endItem 请换成你在 WireItem 里定义的实际变量名！
                        if (wire->m_startItem == comp || wire->m_endItem == comp) {
                            itemsToDelete.insert(wire); // 株连九族，把导线也拉入死亡名单
                        }
                    }
                }
            }
        }

        // 3. 统一执行安全死刑！(先移出舞台，再销毁内存)
        for (QGraphicsItem* item : itemsToDelete) {
            m_scene->removeItem(item);
            delete item; // 此时杀掉导线和元件绝对安全，再也不会闪退了！
        }

        event->accept(); // 拦住事件，防止触发软件退出
        return;
    }

    QMainWindow::keyPressEvent(event);
}
