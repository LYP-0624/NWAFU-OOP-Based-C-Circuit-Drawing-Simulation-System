#include "mainwindow.h"

#include "./ui_mainwindow.h"

#include <QDialog>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFont>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>

#include "Circuit.h"
#include "componentitems.h"
#include "SimulationLogger.h"

// 匿名命名空间：用于封装本文件内部使用的辅助函数，避免与其他文件中的同名函数冲突
namespace {

// 格式化带符号数值，添加单位与可选注释（如电流方向提示）
// value: 数值; unit: 单位字符串; note: 附加说明（如方向提示），默认为空
QString formatSignedValue(double value, const QString& unit, const QString& note = QString()) {
    // 将数值格式化为保留4位小数的字符串，并拼接单位
    QString text = QStringLiteral("%1%2").arg(value, 0, 'f', 4).arg(unit);
    // 如果提供了附加说明，则追加到字符串末尾
    if (!note.isEmpty()) {
        text += QStringLiteral("（%1）").arg(note);
    }
    return text;
}

// 将英文元件类型名转换为中文显示名称
// type: 英文类型标识符（如 "Resistor"）
// 返回对应的中文名称，若未匹配则返回原英文
QString componentTypeToChinese(const QString& type) {
    if (type == QStringLiteral("Resistor")) return QStringLiteral("电阻");
    if (type == QStringLiteral("PowerSource")) return QStringLiteral("电源");
    if (type == QStringLiteral("Bulb")) return QStringLiteral("灯泡");
    if (type == QStringLiteral("Ammeter")) return QStringLiteral("电流表");
    if (type == QStringLiteral("Voltmeter")) return QStringLiteral("电压表");
    if (type == QStringLiteral("Capacitor")) return QStringLiteral("电容");
    if (type == QStringLiteral("Inductor")) return QStringLiteral("电感");
    if (type == QStringLiteral("Switch")) return QStringLiteral("开关");
    return type;
}

// 根据仿真结果数组构建完整的文本报告字符串
// results: 仿真结果向量，包含各元件的电压、电流、功率等信息
// 返回格式化后的完整报告文本
QString buildResultText(const std::vector<CircuitSim::SimulationResult>& results) {
    QString text;
    // 添加报告标题分隔线
    text += QStringLiteral("================================\n");
    text += QStringLiteral("仿真结果\n");
    text += QStringLiteral("================================\n\n");

    // 遍历每个元件的仿真结果
    for (const auto& result : results) {
        // 显示元件中文类型及编号
        text += QStringLiteral("%1 #%2\n")
                    .arg(componentTypeToChinese(QString::fromStdString(result.componentType)))
                    .arg(result.componentId);
        // 显示元件名称
        text += QStringLiteral("名称：%1\n").arg(QString::fromStdString(result.componentName));
        // 显示电压值（带单位）
        text += QStringLiteral("电压：%1\n")
                    .arg(formatSignedValue(result.voltage, QStringLiteral(" V")));

        // 判断电流方向：若电流为负值，说明实际方向与参考方向相反
        QString currentNote;
        if (result.current < -1e-12) {
            currentNote = QStringLiteral("方向与参考方向相反");
        }
        // 显示电流值及方向注释
        text += QStringLiteral("电流：%1\n")
                    .arg(formatSignedValue(result.current, QStringLiteral(" A"), currentNote));

        // 判断功率方向：若功率为负值，说明元件作为电源向外供电
        QString powerNote;
        if (result.power < -1e-12) {
            powerNote = QStringLiteral("电源向外供电");
        }
        // 显示功率值及供电注释
        text += QStringLiteral("功率：%1\n")
                    .arg(formatSignedValue(result.power, QStringLiteral(" W"), powerNote));

        // 若元件处于激活状态，显示工作状态
        if (result.isActive) {
            text += QStringLiteral("状态：工作中\n");
        }

        // 显示额外参数（如电阻值、电容值等）
        if (!result.extra.empty()) {
            text += QStringLiteral("参数：");
            bool first = true;
            for (const auto& [key, value] : result.extra) {
                if (!first) {
                    text += QStringLiteral("，");
                }
                first = false;
                // 将参数名和值拼接，值保留6位小数
                text += QStringLiteral("%1=%2").arg(QString::fromStdString(key)).arg(value, 0, 'f', 6);
            }
            text += QStringLiteral("\n");
        }
        text += QStringLiteral("\n");
    }

    // 添加报告结束分隔线
    text += QStringLiteral("================================\n");
    return text;
}

// 创建并显示仿真结果对话框
// parent: 父窗口指针; results: 仿真结果数据
// componentCount: 元件总数; nodeCount: 节点总数; branchCount: 导线总数
void showSimulationResultDialog(QWidget* parent, const std::vector<CircuitSim::SimulationResult>& results,
                                int componentCount, int nodeCount, int branchCount) {
    // 创建模态对话框，设置标题和尺寸
    QDialog dialog(parent);
    dialog.setWindowTitle(QStringLiteral("仿真结果"));
    dialog.resize(760, 560);

    // 创建垂直布局管理器
    auto* layout = new QVBoxLayout(&dialog);
    // 创建标题标签并设置样式（大字号、粗体、深蓝色）
    auto* title = new QLabel(QStringLiteral("仿真完成"), &dialog);
    title->setStyleSheet(QStringLiteral("font-size: 20px; font-weight: 700; color: #1f3c88;"));

    // 创建摘要标签，显示电路统计信息
    auto* summary = new QLabel(
        QStringLiteral("元件 %1 个，节点 %2 个，导线 %3 条")
            .arg(componentCount)
            .arg(nodeCount)
            .arg(branchCount),
        &dialog);
    summary->setStyleSheet(QStringLiteral("color: #4b5563;"));

    // 创建只读文本编辑器用于显示详细结果
    auto* viewer = new QTextEdit(&dialog);
    viewer->setReadOnly(true);
    viewer->setFont(QFont(QStringLiteral("Consolas"), 10));
    viewer->setPlainText(buildResultText(results));
    // 设置文本编辑器样式（浅色背景、圆角边框、内边距）
    viewer->setStyleSheet(QStringLiteral(
        "QTextEdit {"
        "  background: #fbfcff;"
        "  border: 1px solid #c9d6ff;"
        "  border-radius: 10px;"
        "  padding: 10px;"
        "}"));

    // 创建关闭按钮组
    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    // 连接关闭按钮信号到对话框的拒绝（关闭）槽
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    // 将各控件添加到布局中（viewer 权重为1，自动填充剩余空间）
    layout->addWidget(title);
    layout->addWidget(summary);
    layout->addWidget(viewer, 1);
    layout->addWidget(buttons);

    // 以模态方式显示对话框，阻塞直到关闭
    dialog.exec();
}

} // namespace

// 构造函数：初始化主窗口
// parent: 父窗口指针，默认为 nullptr
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_scene(new CircuitScene(this)),
      m_circuit(std::make_unique<CircuitSim::Circuit>()) {
    // 设置UI界面
    ui->setupUi(this);
    // 设置窗口标题
    setWindowTitle(QStringLiteral("简易电路图生成与仿真系统"));
    // 设置窗口初始大小
    resize(1280, 860);

    // 将电路模型指针传递给图形组件和场景，建立数据与视图的关联
    GraphComponent::setCircuit(m_circuit.get());
    m_scene->setCircuit(m_circuit.get());
    // 将场景设置到视图控件中
    ui->circuitView->setScene(m_scene);
    // 启用抗锯齿渲染，使图形边缘更平滑
    ui->circuitView->setRenderHint(QPainter::Antialiasing);
    // 设置拖拽模式为橡皮筋框选（支持多选）
    ui->circuitView->setDragMode(QGraphicsView::RubberBandDrag);

    // 初始化菜单栏、工具栏和信号连接
    setupMenuBar();
    setupToolBar();
    setupConnections();

    // 设置左侧工具栏各按钮的中文显示文本
    ui->btnResistor->setText(QStringLiteral("电阻"));
    ui->btnPower->setText(QStringLiteral("电源"));
    ui->btnBulb->setText(QStringLiteral("灯泡"));
    ui->btnAmmeter->setText(QStringLiteral("电流表"));
    ui->btnVoltmeter->setText(QStringLiteral("电压表"));
    ui->capacitor->setText(QStringLiteral("电容"));
    ui->inductor->setText(QStringLiteral("电感"));
    ui->switch1->setText(QStringLiteral("开关"));

    // 在状态栏显示初始操作提示信息
    statusBar()->showMessage(QStringLiteral("点击左侧元件按钮进入放置模式，放置一个元件后会自动退出；按 ESC 或右键可取消。"));
}

// 析构函数：释放UI资源
MainWindow::~MainWindow() {
    delete ui;
}

// 设置菜单栏：创建文件、仿真、帮助三个菜单
void MainWindow::setupMenuBar() {
    // 创建"文件"菜单
    auto* fileMenu = menuBar()->addMenu(tr("文件"));
    // 添加"新建"动作，绑定快捷键 Ctrl+N
    fileMenu->addAction(tr("新建"), QKeySequence::New, this, &MainWindow::onNewCircuit);
    // 添加"保存"动作，绑定快捷键 Ctrl+S
    fileMenu->addAction(tr("保存"), QKeySequence::Save, this, &MainWindow::onSaveCircuit);
    // 添加"打开"动作，绑定快捷键 Ctrl+O
    fileMenu->addAction(tr("打开"), QKeySequence::Open, this, &MainWindow::onLoadCircuit);
    fileMenu->addSeparator();
    // 添加"退出"动作，绑定快捷键 Ctrl+Q
    fileMenu->addAction(tr("退出"), QKeySequence::Quit, this, &QWidget::close);

    // 创建"仿真"菜单
    auto* simMenu = menuBar()->addMenu(tr("仿真"));
    // 添加"运行"动作，绑定空格键
    simMenu->addAction(tr("运行"), QKeySequence(Qt::Key_Space), this, &MainWindow::onSolveCircuit);
    // 添加"清空选择"动作
    simMenu->addAction(tr("清空选择"), this, &MainWindow::onClearAll);

    // 创建"帮助"菜单
    auto* helpMenu = menuBar()->addMenu(tr("帮助"));
    helpMenu->addAction(tr("使用说明"), this, &MainWindow::onShowHelp);
    helpMenu->addAction(tr("关于"), this, &MainWindow::onAbout);
}

// 设置工具栏（当前为空实现，预留扩展接口）
void MainWindow::setupToolBar() {
}

// 设置信号与槽连接：将左侧工具栏按钮的点击信号绑定到对应的槽函数
void MainWindow::setupConnections() {
    connect(ui->btnResistor, &QPushButton::clicked, this, &MainWindow::onAddResistor);
    connect(ui->btnPower, &QPushButton::clicked, this, &MainWindow::onAddPowerSource);
    connect(ui->btnBulb, &QPushButton::clicked, this, &MainWindow::onAddBulb);
    connect(ui->btnAmmeter, &QPushButton::clicked, this, &MainWindow::onAddAmmeter);
    connect(ui->btnVoltmeter, &QPushButton::clicked, this, &MainWindow::onAddVoltmeter);
    connect(ui->capacitor, &QPushButton::clicked, this, &MainWindow::onAddCapacitor);
    connect(ui->inductor, &QPushButton::clicked, this, &MainWindow::onAddInductor);
    connect(ui->switch1, &QPushButton::clicked, this, &MainWindow::onAddSwitch);
}

// 在状态栏显示工具提示信息，3秒后自动消失
// text: 要显示的提示文本
void MainWindow::updateStatusForTool(const QString& text) {
    statusBar()->showMessage(text, 3000);
}

// 重置电路：创建新的电路模型，并重新建立视图与数据的关联
void MainWindow::resetCircuit() {
    m_circuit = std::make_unique<CircuitSim::Circuit>();
    GraphComponent::setCircuit(m_circuit.get());
    m_scene->setCircuit(m_circuit.get());
    // 同步场景显示与新的电路数据
    m_scene->syncFromCircuit();
}

// 键盘事件处理：支持 ESC 取消放置、空格运行仿真、Delete/Backspace 删除选中项
// event: 键盘事件对象
void MainWindow::keyPressEvent(QKeyEvent* event) {
    // ESC 键：取消当前元件放置模式
    if (event->key() == Qt::Key_Escape) {
        m_scene->cancelPlacementMode();
        updateStatusForTool(QStringLiteral("已取消放置模式。"));
        event->accept();
        return;
    }
    // 空格键：执行电路仿真
    if (event->key() == Qt::Key_Space) {
        performSimulation();
        event->accept();
        return;
    }
    // Delete 或 Backspace 键：删除当前选中的元件或导线
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        m_scene->deleteSelectedItems();
        event->accept();
        return;
    }
    // 其他按键交由父类默认处理
    QMainWindow::keyPressEvent(event);
}

// 槽函数：设置场景工具为"电阻放置模式"
void MainWindow::onAddResistor() {
    m_scene->setTool(CircuitScene::Tool::Resistor);
    updateStatusForTool(QStringLiteral("放置模式：电阻。点击画布放置一个元件后会自动退出。"));
}

// 槽函数：设置场景工具为"电源放置模式"
void MainWindow::onAddPowerSource() {
    m_scene->setTool(CircuitScene::Tool::PowerSource);
    updateStatusForTool(QStringLiteral("放置模式：电源。点击画布放置一个元件后会自动退出。"));
}

// 槽函数：设置场景工具为"灯泡放置模式"
void MainWindow::onAddBulb() {
    m_scene->setTool(CircuitScene::Tool::Bulb);
    updateStatusForTool(QStringLiteral("放置模式：灯泡。点击画布放置一个元件后会自动退出。"));
}

// 槽函数：设置场景工具为"电流表放置模式"
void MainWindow::onAddAmmeter() {
    m_scene->setTool(CircuitScene::Tool::Ammeter);
    updateStatusForTool(QStringLiteral("放置模式：电流表。点击画布放置一个元件后会自动退出。"));
}

// 槽函数：设置场景工具为"电压表放置模式"
void MainWindow::onAddVoltmeter() {
    m_scene->setTool(CircuitScene::Tool::Voltmeter);
    updateStatusForTool(QStringLiteral("放置模式：电压表。点击画布放置一个元件后会自动退出。"));
}

// 槽函数：设置场景工具为"电容放置模式"
void MainWindow::onAddCapacitor() {
    m_scene->setTool(CircuitScene::Tool::Capacitor);
    updateStatusForTool(QStringLiteral("放置模式：电容。点击画布放置一个元件后会自动退出。"));
}

// 槽函数：设置场景工具为"电感放置模式"
void MainWindow::onAddInductor() {
    m_scene->setTool(CircuitScene::Tool::Inductor);
    updateStatusForTool(QStringLiteral("放置模式：电感。点击画布放置一个元件后会自动退出。"));
}

// 槽函数：设置场景工具为"开关放置模式"
void MainWindow::onAddSwitch() {
    m_scene->setTool(CircuitScene::Tool::Switch);
    updateStatusForTool(QStringLiteral("放置模式：开关。点击画布放置一个元件后会自动退出。"));
}

// 执行电路仿真：求解电路并显示结果
void MainWindow::performSimulation() {
    // 调用电路求解器，若求解失败则弹出警告
    if (!m_circuit->solve()) {
        QMessageBox::warning(this, QStringLiteral("仿真失败"),
                             QStringLiteral("电路无法求解：\n%1")
                                 .arg(QString::fromStdString(m_circuit->getLastError())));
        statusBar()->showMessage(QStringLiteral("仿真失败：%1").arg(QString::fromStdString(m_circuit->getLastError())), 5000);
        return;
    }

    // 刷新场景中各元件的视觉状态（如灯泡亮灭）
    m_scene->refreshVisuals();
    // 记录本次仿真日志
    m_circuit->logSimulation("Manual solve");

    // ========== 新增：记录文本日志并导出 CSV ==========
    CircuitSim::SimulationLogger& logger = CircuitSim::SimulationLogger::getInstance();
    logger.logInfo("Simulation completed successfully");  // ← 生成 simulation_log.txt
    bool exportOk = logger.exportCSV("simulation_results.csv");  // ← 生成 CSV
    if (exportOk) {
        statusBar()->showMessage(QStringLiteral("仿真完成，CSV 已导出"), 3000);
    } else {
        statusBar()->showMessage(QStringLiteral("仿真完成，CSV 导出失败"), 3000);
    }
    // ==================================================
    
    // 获取仿真结果并显示结果对话框
    const auto results = m_circuit->getResults();
    showSimulationResultDialog(this, results, m_circuit->getComponentCount(), m_circuit->getNodeCount(),
                               m_circuit->getBranchCount());

    // 在状态栏显示仿真完成摘要信息，5秒后消失
    statusBar()->showMessage(
        QStringLiteral("仿真完成：%1 个元件，%2 个节点，%3 条导线。")
            .arg(m_circuit->getComponentCount())
            .arg(m_circuit->getNodeCount())
            .arg(m_circuit->getBranchCount()),
        5000);
}

// 槽函数：新建电路（清空当前画布）
void MainWindow::onNewCircuit() {
    // 弹出确认对话框，用户确认后才清空
    if (QMessageBox::question(this, QStringLiteral("新建电路"), QStringLiteral("是否清空当前电路？")) != QMessageBox::Yes) {
        return;
    }
    resetCircuit();
    statusBar()->showMessage(QStringLiteral("已创建新的空电路。"), 3000);
}

// 槽函数：保存当前电路到 JSON 文件
void MainWindow::onSaveCircuit() {
    // 弹出保存文件对话框，默认过滤 JSON 文件
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("保存电路"), QString(),
                                                    QStringLiteral("JSON 文件 (*.json)"));
    if (filePath.isEmpty()) {
        return;
    }
    // 若文件名未带 .json 后缀，自动追加
    if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
    }

    // 调用电路模型的保存方法，失败则弹出警告
    if (!m_circuit->saveToJsonFile(filePath.toStdString())) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), QStringLiteral("无法保存当前电路。"));
        return;
    }
    statusBar()->showMessage(QStringLiteral("已保存到 %1").arg(filePath), 4000);
}

// 槽函数：从 JSON 文件加载电路
void MainWindow::onLoadCircuit() {
    // 弹出打开文件对话框
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("打开电路"), QString(),
                                                    QStringLiteral("JSON 文件 (*.json)"));
    if (filePath.isEmpty()) {
        return;
    }

    // 创建临时电路对象尝试加载
    auto loaded = std::make_unique<CircuitSim::Circuit>();
    if (!loaded->loadFromJsonFile(filePath.toStdString())) {
        QMessageBox::warning(this, QStringLiteral("打开失败"), QStringLiteral("无法加载所选文件。"));
        return;
    }

    // 加载成功，替换当前电路并同步视图
    m_circuit = std::move(loaded);
    GraphComponent::setCircuit(m_circuit.get());
    m_scene->setCircuit(m_circuit.get());
    m_scene->syncFromCircuit();
    statusBar()->showMessage(QStringLiteral("已加载 %1").arg(filePath), 4000);
}

// 槽函数：菜单栏"运行仿真"的入口，调用 performSimulation
void MainWindow::onSolveCircuit() {
    performSimulation();
}

// 槽函数：清空画布上的所有元件和导线
void MainWindow::onClearAll() {
    // 弹出确认对话框
    if (QMessageBox::question(this, QStringLiteral("清空"), QStringLiteral("是否移除画布中的所有元件？")) != QMessageBox::Yes) {
        return;
    }
    resetCircuit();
    statusBar()->showMessage(QStringLiteral("已清空画布。"), 3000);
}

// 槽函数：显示使用说明对话框
void MainWindow::onShowHelp() {
    QMessageBox::information(
        this, QStringLiteral("使用说明"),
        QStringLiteral("1. 点击左侧元件按钮进入放置模式。\n"
                       "2. 在画布上点击一次即可放置一个元件，放置后会自动退出。\n"
                       "3. 在元件端点之间拖拽可创建导线。\n"
                       "4. 选中元件或导线后按 Delete 可删除。\n"
                       "5. 双击元件可编辑参数。\n"
                       "6. 按空格键或使用“仿真 -> 运行”可查看结果。"));
}

// 槽函数：显示关于对话框，展示程序信息
void MainWindow::onAbout() {
    QMessageBox::about(this, QStringLiteral("关于"),
                       QStringLiteral("简易电路图生成与仿真系统\nQt6 + C++17 课程实习项目。"));
}