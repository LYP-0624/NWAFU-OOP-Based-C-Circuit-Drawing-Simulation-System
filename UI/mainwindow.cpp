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

namespace {

QString formatSignedValue(double value, const QString& unit, const QString& note = QString()) {
    QString text = QStringLiteral("%1%2").arg(value, 0, 'f', 4).arg(unit);
    if (!note.isEmpty()) {
        text += QStringLiteral("（%1）").arg(note);
    }
    return text;
}

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

QString buildResultText(const std::vector<CircuitSim::SimulationResult>& results) {
    QString text;
    text += QStringLiteral("================================\n");
    text += QStringLiteral("仿真结果\n");
    text += QStringLiteral("================================\n\n");

    for (const auto& result : results) {
        text += QStringLiteral("%1 #%2\n")
                    .arg(componentTypeToChinese(QString::fromStdString(result.componentType)))
                    .arg(result.componentId);
        text += QStringLiteral("名称：%1\n").arg(QString::fromStdString(result.componentName));
        text += QStringLiteral("电压：%1\n")
                    .arg(formatSignedValue(result.voltage, QStringLiteral(" V")));

        QString currentNote;
        if (result.current < -1e-12) {
            currentNote = QStringLiteral("方向与参考方向相反");
        }
        text += QStringLiteral("电流：%1\n")
                    .arg(formatSignedValue(result.current, QStringLiteral(" A"), currentNote));

        QString powerNote;
        if (result.power < -1e-12) {
            powerNote = QStringLiteral("电源向外供电");
        }
        text += QStringLiteral("功率：%1\n")
                    .arg(formatSignedValue(result.power, QStringLiteral(" W"), powerNote));

        if (result.isActive) {
            text += QStringLiteral("状态：工作中\n");
        }

        if (!result.extra.empty()) {
            text += QStringLiteral("参数：");
            bool first = true;
            for (const auto& [key, value] : result.extra) {
                if (!first) {
                    text += QStringLiteral("，");
                }
                first = false;
                text += QStringLiteral("%1=%2").arg(QString::fromStdString(key)).arg(value, 0, 'f', 6);
            }
            text += QStringLiteral("\n");
        }
        text += QStringLiteral("\n");
    }

    text += QStringLiteral("================================\n");
    return text;
}

void showSimulationResultDialog(QWidget* parent, const std::vector<CircuitSim::SimulationResult>& results,
                                int componentCount, int nodeCount, int branchCount) {
    QDialog dialog(parent);
    dialog.setWindowTitle(QStringLiteral("仿真结果"));
    dialog.resize(760, 560);

    auto* layout = new QVBoxLayout(&dialog);
    auto* title = new QLabel(QStringLiteral("仿真完成"), &dialog);
    title->setStyleSheet(QStringLiteral("font-size: 20px; font-weight: 700; color: #1f3c88;"));

    auto* summary = new QLabel(
        QStringLiteral("元件 %1 个，节点 %2 个，导线 %3 条")
            .arg(componentCount)
            .arg(nodeCount)
            .arg(branchCount),
        &dialog);
    summary->setStyleSheet(QStringLiteral("color: #4b5563;"));

    auto* viewer = new QTextEdit(&dialog);
    viewer->setReadOnly(true);
    viewer->setFont(QFont(QStringLiteral("Consolas"), 10));
    viewer->setPlainText(buildResultText(results));
    viewer->setStyleSheet(QStringLiteral(
        "QTextEdit {"
        "  background: #fbfcff;"
        "  border: 1px solid #c9d6ff;"
        "  border-radius: 10px;"
        "  padding: 10px;"
        "}"));

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Close, &dialog);
    QObject::connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    layout->addWidget(title);
    layout->addWidget(summary);
    layout->addWidget(viewer, 1);
    layout->addWidget(buttons);

    dialog.exec();
}

} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), m_scene(new CircuitScene(this)),
      m_circuit(std::make_unique<CircuitSim::Circuit>()) {
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("简易电路图生成与仿真系统"));
    resize(1280, 860);

    GraphComponent::setCircuit(m_circuit.get());
    m_scene->setCircuit(m_circuit.get());
    ui->circuitView->setScene(m_scene);
    ui->circuitView->setRenderHint(QPainter::Antialiasing);
    ui->circuitView->setDragMode(QGraphicsView::RubberBandDrag);

    setupMenuBar();
    setupToolBar();
    setupConnections();

    ui->btnResistor->setText(QStringLiteral("电阻"));
    ui->btnPower->setText(QStringLiteral("电源"));
    ui->btnBulb->setText(QStringLiteral("灯泡"));
    ui->btnAmmeter->setText(QStringLiteral("电流表"));
    ui->btnVoltmeter->setText(QStringLiteral("电压表"));
    ui->capacitor->setText(QStringLiteral("电容"));
    ui->inductor->setText(QStringLiteral("电感"));
    ui->switch1->setText(QStringLiteral("开关"));

    statusBar()->showMessage(QStringLiteral("点击左侧元件按钮进入放置模式，放置一个元件后会自动退出；按 ESC 或右键可取消。"));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::setupMenuBar() {
    auto* fileMenu = menuBar()->addMenu(tr("文件"));
    fileMenu->addAction(tr("新建"), QKeySequence::New, this, &MainWindow::onNewCircuit);
    fileMenu->addAction(tr("保存"), QKeySequence::Save, this, &MainWindow::onSaveCircuit);
    fileMenu->addAction(tr("打开"), QKeySequence::Open, this, &MainWindow::onLoadCircuit);
    fileMenu->addSeparator();
    fileMenu->addAction(tr("退出"), QKeySequence::Quit, this, &QWidget::close);

    auto* simMenu = menuBar()->addMenu(tr("仿真"));
    simMenu->addAction(tr("运行"), QKeySequence(Qt::Key_Space), this, &MainWindow::onSolveCircuit);
    simMenu->addAction(tr("清空选择"), this, &MainWindow::onClearAll);

    auto* helpMenu = menuBar()->addMenu(tr("帮助"));
    helpMenu->addAction(tr("使用说明"), this, &MainWindow::onShowHelp);
    helpMenu->addAction(tr("关于"), this, &MainWindow::onAbout);
}

void MainWindow::setupToolBar() {
}

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

void MainWindow::updateStatusForTool(const QString& text) {
    statusBar()->showMessage(text, 3000);
}

void MainWindow::resetCircuit() {
    m_circuit = std::make_unique<CircuitSim::Circuit>();
    GraphComponent::setCircuit(m_circuit.get());
    m_scene->setCircuit(m_circuit.get());
    m_scene->syncFromCircuit();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Escape) {
        m_scene->cancelPlacementMode();
        updateStatusForTool(QStringLiteral("已取消放置模式。"));
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Space) {
        performSimulation();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Delete || event->key() == Qt::Key_Backspace) {
        m_scene->deleteSelectedItems();
        event->accept();
        return;
    }
    QMainWindow::keyPressEvent(event);
}

void MainWindow::onAddResistor() {
    m_scene->setTool(CircuitScene::Tool::Resistor);
    updateStatusForTool(QStringLiteral("放置模式：电阻。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::onAddPowerSource() {
    m_scene->setTool(CircuitScene::Tool::PowerSource);
    updateStatusForTool(QStringLiteral("放置模式：电源。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::onAddBulb() {
    m_scene->setTool(CircuitScene::Tool::Bulb);
    updateStatusForTool(QStringLiteral("放置模式：灯泡。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::onAddAmmeter() {
    m_scene->setTool(CircuitScene::Tool::Ammeter);
    updateStatusForTool(QStringLiteral("放置模式：电流表。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::onAddVoltmeter() {
    m_scene->setTool(CircuitScene::Tool::Voltmeter);
    updateStatusForTool(QStringLiteral("放置模式：电压表。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::onAddCapacitor() {
    m_scene->setTool(CircuitScene::Tool::Capacitor);
    updateStatusForTool(QStringLiteral("放置模式：电容。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::onAddInductor() {
    m_scene->setTool(CircuitScene::Tool::Inductor);
    updateStatusForTool(QStringLiteral("放置模式：电感。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::onAddSwitch() {
    m_scene->setTool(CircuitScene::Tool::Switch);
    updateStatusForTool(QStringLiteral("放置模式：开关。点击画布放置一个元件后会自动退出。"));
}

void MainWindow::performSimulation() {
    if (!m_circuit->solve()) {
        QMessageBox::warning(this, QStringLiteral("仿真失败"),
                             QStringLiteral("电路无法求解：\n%1")
                                 .arg(QString::fromStdString(m_circuit->getLastError())));
        statusBar()->showMessage(QStringLiteral("仿真失败：%1").arg(QString::fromStdString(m_circuit->getLastError())), 5000);
        return;
    }

    m_scene->refreshVisuals();
    m_circuit->logSimulation("Manual solve");

    const auto results = m_circuit->getResults();
    showSimulationResultDialog(this, results, m_circuit->getComponentCount(), m_circuit->getNodeCount(),
                               m_circuit->getBranchCount());

    statusBar()->showMessage(
        QStringLiteral("仿真完成：%1 个元件，%2 个节点，%3 条导线。")
            .arg(m_circuit->getComponentCount())
            .arg(m_circuit->getNodeCount())
            .arg(m_circuit->getBranchCount()),
        5000);
}

void MainWindow::onNewCircuit() {
    if (QMessageBox::question(this, QStringLiteral("新建电路"), QStringLiteral("是否清空当前电路？")) != QMessageBox::Yes) {
        return;
    }
    resetCircuit();
    statusBar()->showMessage(QStringLiteral("已创建新的空电路。"), 3000);
}

void MainWindow::onSaveCircuit() {
    QString filePath = QFileDialog::getSaveFileName(this, QStringLiteral("保存电路"), QString(),
                                                    QStringLiteral("JSON 文件 (*.json)"));
    if (filePath.isEmpty()) {
        return;
    }
    if (!filePath.endsWith(".json", Qt::CaseInsensitive)) {
        filePath += ".json";
    }

    if (!m_circuit->saveToJsonFile(filePath.toStdString())) {
        QMessageBox::warning(this, QStringLiteral("保存失败"), QStringLiteral("无法保存当前电路。"));
        return;
    }
    statusBar()->showMessage(QStringLiteral("已保存到 %1").arg(filePath), 4000);
}

void MainWindow::onLoadCircuit() {
    QString filePath = QFileDialog::getOpenFileName(this, QStringLiteral("打开电路"), QString(),
                                                    QStringLiteral("JSON 文件 (*.json)"));
    if (filePath.isEmpty()) {
        return;
    }

    auto loaded = std::make_unique<CircuitSim::Circuit>();
    if (!loaded->loadFromJsonFile(filePath.toStdString())) {
        QMessageBox::warning(this, QStringLiteral("打开失败"), QStringLiteral("无法加载所选文件。"));
        return;
    }

    m_circuit = std::move(loaded);
    GraphComponent::setCircuit(m_circuit.get());
    m_scene->setCircuit(m_circuit.get());
    m_scene->syncFromCircuit();
    statusBar()->showMessage(QStringLiteral("已加载 %1").arg(filePath), 4000);
}

void MainWindow::onSolveCircuit() {
    performSimulation();
}

void MainWindow::onClearAll() {
    if (QMessageBox::question(this, QStringLiteral("清空"), QStringLiteral("是否移除画布中的所有元件？")) != QMessageBox::Yes) {
        return;
    }
    resetCircuit();
    statusBar()->showMessage(QStringLiteral("已清空画布。"), 3000);
}

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

void MainWindow::onAbout() {
    QMessageBox::about(this, QStringLiteral("关于"),
                       QStringLiteral("简易电路图生成与仿真系统\nQt6 + C++17 课程实习项目。"));
}
