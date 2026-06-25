#include "graphcomponent.h"

#include <QApplication>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QGraphicsScene>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QLabel>
#include <QLineF>
#include <QPainter>
#include <QPen>
#include <QStringList>
#include <QVBoxLayout>

#include "Ammeter.h"
#include "Bulb.h"
#include "Capacitor.h"
#include "Circuit.h"
#include "Inductor.h"
#include "Node.h"
#include "Port.h"
#include "PowerSource.h"
#include "Resistor.h"
#include "Switch.h"
#include "Voltmeter.h"
#include "wireitem.h"

// 静态成员：指向当前电路模型的指针
CircuitSim::Circuit* GraphComponent::s_circuit = nullptr;

namespace {

// 将英文类型名转换为显示标签
QString typeLabel(const QString& typeName) {
    if (typeName == QStringLiteral("Resistor")) return QStringLiteral("Resistor");
    if (typeName == QStringLiteral("PowerSource")) return QStringLiteral("Power Source");
    if (typeName == QStringLiteral("Bulb")) return QStringLiteral("Bulb");
    if (typeName == QStringLiteral("Ammeter")) return QStringLiteral("Ammeter");
    if (typeName == QStringLiteral("Voltmeter")) return QStringLiteral("Voltmeter");
    if (typeName == QStringLiteral("Capacitor")) return QStringLiteral("Capacitor");
    if (typeName == QStringLiteral("Inductor")) return QStringLiteral("Inductor");
    if (typeName == QStringLiteral("Switch")) return QStringLiteral("Switch");
    return typeName;
}

// 生成元器件端口连接信息文本
QString nodeInfoText(const CircuitSim::Component* comp) {
    if (!comp) {
        return QStringLiteral("No component loaded");
    }

    QStringList parts;
    const auto ports = comp->getPorts();
    for (size_t i = 0; i < ports.size(); ++i) {
        const auto* port = ports[i];
        if (!port) {
            parts << QStringLiteral("P%1: -").arg(i);
            continue;
        }
        const auto* node = port->getNode();
        parts << QStringLiteral("P%1: %2")
                     .arg(i)
                     .arg(node ? QStringLiteral("N%1").arg(node->getId()) : QStringLiteral("-"));
    }
    return parts.join(QStringLiteral("  "));
}

// 编辑后刷新电路仿真和图形显示
void refreshCircuitAfterEdit(GraphComponent* item) {
    if (!item) {
        return;
    }

    if (GraphComponent::s_circuit) {
        GraphComponent::s_circuit->solve();
    }
    if (auto* sc = item->scene()) {
        sc->update();
    }
    item->update();
    item->updateConnectedWires();
}

// 元器件属性编辑对话框
class ComponentPropertyDialog : public QDialog {
public:
    // 构造函数：创建属性编辑界面
    explicit ComponentPropertyDialog(GraphComponent* item, QWidget* parent = nullptr)
        : QDialog(parent), m_item(item) {
        setWindowTitle(QStringLiteral("Component Properties"));
        setModal(true);
        resize(460, 280);

        auto* rootLayout = new QVBoxLayout(this);
        auto* form = new QFormLayout();

        // 显示基本信息（类型、ID、名称、节点连接）
        auto* typeValue = new QLabel(typeLabel(item->typeName()));
        auto* idValue = new QLabel(QString::number(item->id()));
        auto* nameValue = new QLabel(item->model() ? QString::fromStdString(item->model()->getName()) : QString());
        auto* nodeValue = new QLabel(nodeInfoText(item->model()));
        nodeValue->setWordWrap(true);

        form->addRow(QStringLiteral("Type"), typeValue);
        form->addRow(QStringLiteral("ID"), idValue);
        form->addRow(QStringLiteral("Name"), nameValue);

        // 根据元器件类型构建对应的编辑器
        buildEditor(form);

        auto* nodeLabel = new QLabel(nodeValue->text());
        nodeLabel->setWordWrap(true);
        form->addRow(QStringLiteral("Nodes"), nodeLabel);

        rootLayout->addLayout(form);

        // 添加确定/取消按钮
        auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
        connect(buttons, &QDialogButtonBox::accepted, this, &ComponentPropertyDialog::accept);
        connect(buttons, &QDialogButtonBox::rejected, this, &ComponentPropertyDialog::reject);
        rootLayout->addWidget(buttons);
    }

    // 确认时应用修改
    void accept() override {
        applyChanges();
        QDialog::accept();
    }

private:
    // 根据元器件类型创建对应的属性编辑器
    void buildEditor(QFormLayout* form) {
        auto* comp = m_item ? m_item->model() : nullptr;
        const QString type = m_item ? m_item->typeName() : QString();

        if (type == QStringLiteral("Resistor")) {
            // 电阻：编辑电阻值
            m_valueSpin = createValueSpin(1e-12, 1e12, 100.0, 1);
            if (const auto* r = dynamic_cast<const CircuitSim::Resistor*>(comp)) {
                m_valueSpin->setValue(r->getResistance());
            }
            form->addRow(QStringLiteral("Resistance (Ohm)"), m_valueSpin);
            return;
        }

        if (type == QStringLiteral("PowerSource")) {
            // 电源：编辑类型（电压/电流）和数值
            m_sourceType = new QComboBox(this);
            m_sourceType->addItem(QStringLiteral("Voltage Source"));
            m_sourceType->addItem(QStringLiteral("Current Source"));
            const auto* src = dynamic_cast<const CircuitSim::PowerSource*>(comp);
            m_sourceType->setCurrentIndex(src && src->isCurrentSource() ? 1 : 0);
            connect(m_sourceType, QOverload<int>::of(&QComboBox::currentIndexChanged),
                    this, [this](int) { updateSourceLabel(); });

            m_valueSpin = createValueSpin(-1e12, 1e12, 5.0, 6);
            if (const auto* srcModel = dynamic_cast<const CircuitSim::PowerSource*>(comp)) {
                m_valueSpin->setValue(srcModel->getValue());
            }
            updateSourceLabel();

            form->addRow(QStringLiteral("Source Type"), m_sourceType);
            form->addRow(QStringLiteral("Value"), m_valueSpin);
            return;
        }

        if (type == QStringLiteral("Bulb")) {
            // 灯泡：编辑电阻值和额定功率
            m_valueSpin = createValueSpin(1e-12, 1e12, 10.0, 1);
            if (const auto* bulb = dynamic_cast<const CircuitSim::Bulb*>(comp)) {
                m_valueSpin->setValue(bulb->getResistance());
                m_ratedPowerSpin = createValueSpin(1e-12, 1e12, bulb->getRatedPower(), 6);
            } else {
                m_ratedPowerSpin = createValueSpin(1e-12, 1e12, 0.5, 6);
            }
            form->addRow(QStringLiteral("Resistance (Ohm)"), m_valueSpin);
            form->addRow(QStringLiteral("Rated Power (W)"), m_ratedPowerSpin);
            return;
        }

        if (type == QStringLiteral("Capacitor")) {
            // 电容：编辑电容值
            m_valueSpin = createValueSpin(1e-12, 1e12, 1e-6, 9);
            if (const auto* cap = dynamic_cast<const CircuitSim::Capacitor*>(comp)) {
                m_valueSpin->setValue(cap->getCapacitance());
            }
            form->addRow(QStringLiteral("Capacitance (F)"), m_valueSpin);
            return;
        }

        if (type == QStringLiteral("Inductor")) {
            // 电感：编辑电感值
            m_valueSpin = createValueSpin(1e-12, 1e12, 1e-3, 9);
            if (const auto* ind = dynamic_cast<const CircuitSim::Inductor*>(comp)) {
                m_valueSpin->setValue(ind->getInductance());
            }
            form->addRow(QStringLiteral("Inductance (H)"), m_valueSpin);
            return;
        }

        if (type == QStringLiteral("Ammeter") || type == QStringLiteral("Voltmeter")) {
            // 测量仪表：只读
            auto* note = new QLabel(QStringLiteral("Measurement components are read-only."));
            note->setWordWrap(true);
            form->addRow(QStringLiteral("Note"), note);
            return;
        }

        // 其他类型：无编辑属性
        auto* note = new QLabel(QStringLiteral("This component has no editable properties."));
        note->setWordWrap(true);
        form->addRow(QStringLiteral("Note"), note);
    }

    // 创建数值输入框
    QDoubleSpinBox* createValueSpin(double minValue, double maxValue, double defaultValue, int decimals) {
        auto* spin = new QDoubleSpinBox(this);
        spin->setRange(minValue, maxValue);
        spin->setDecimals(decimals);
        spin->setSingleStep(1.0);
        spin->setValue(defaultValue);
        return spin;
    }

    // 根据电源类型更新数值单位后缀
    void updateSourceLabel() {
        if (!m_sourceType || !m_valueSpin) {
            return;
        }
        const bool currentMode = m_sourceType->currentIndex() == 1;
        m_valueSpin->setSuffix(currentMode ? QStringLiteral(" A") : QStringLiteral(" V"));
    }

    // 将编辑后的值应用到电路模型
    void applyChanges() {
        auto* comp = m_item ? m_item->model() : nullptr;
        if (!comp) {
            return;
        }

        if (auto* r = dynamic_cast<CircuitSim::Resistor*>(comp); r && m_valueSpin) {
            const double value = m_valueSpin->value();
            r->setResistance(value);
            r->setProperty("resistance", value);
        } else if (auto* src = dynamic_cast<CircuitSim::PowerSource*>(comp); src && m_valueSpin) {
            const bool currentMode = m_sourceType && m_sourceType->currentIndex() == 1;
            src->setSourceType(currentMode ? CircuitSim::SourceType::CURRENT_SOURCE
                                           : CircuitSim::SourceType::VOLTAGE_SOURCE);
            src->setValue(m_valueSpin->value());
            src->setProperty("value", m_valueSpin->value());
            src->setProperty("sourceType", currentMode ? 1.0 : 0.0);
        } else if (auto* bulb = dynamic_cast<CircuitSim::Bulb*>(comp); bulb && m_valueSpin) {
            const double resistance = m_valueSpin->value();
            bulb->setResistance(resistance);
            bulb->setProperty("resistance", resistance);
            if (m_ratedPowerSpin) {
                bulb->setRatedPower(m_ratedPowerSpin->value());
                bulb->setProperty("ratedPower", m_ratedPowerSpin->value());
            }
        } else if (auto* cap = dynamic_cast<CircuitSim::Capacitor*>(comp); cap && m_valueSpin) {
            const double value = m_valueSpin->value();
            cap->setCapacitance(value);
            cap->setProperty("capacitance", value);
        } else if (auto* ind = dynamic_cast<CircuitSim::Inductor*>(comp); ind && m_valueSpin) {
            const double value = m_valueSpin->value();
            ind->setInductance(value);
            ind->setProperty("inductance", value);
        }

        refreshCircuitAfterEdit(m_item);
    }

    GraphComponent* m_item = nullptr;
    QDoubleSpinBox* m_valueSpin = nullptr;
    QDoubleSpinBox* m_ratedPowerSpin = nullptr;
    QComboBox* m_sourceType = nullptr;
};

} // namespace

// 构造函数：初始化图形项，设置可移动、可选中、发送几何变化通知
GraphComponent::GraphComponent(int id, const QString& type, QGraphicsItem* parent)
    : QGraphicsObject(parent), m_id(id), m_type(type), m_hovered(false) {
    setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges);
    setAcceptHoverEvents(true);
    setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

// 设置当前电路模型（静态方法）
void GraphComponent::setCircuit(CircuitSim::Circuit* c) {
    s_circuit = c;
}

// 返回图形项的边界矩形
QRectF GraphComponent::boundingRect() const {
    return QRectF(-54, -36, 108, 108);
}

// 返回指定端口的局部坐标位置
QPointF GraphComponent::terminalLocalPos(int terminal) const {
    return terminal == 0 ? QPointF(-40, 0) : QPointF(40, 0);
}

// 返回指定端口的场景坐标位置
QPointF GraphComponent::terminalScenePos(int terminal) const {
    return mapToScene(terminalLocalPos(terminal));
}

// 判断场景坐标是否在某个端口附近，返回端口索引或-1
int GraphComponent::terminalAt(const QPointF& scenePos, qreal radius) const {
    const QPointF local = mapFromScene(scenePos);
    for (int terminal = 0; terminal < 2; ++terminal) {
        if (QLineF(local, terminalLocalPos(terminal)).length() <= radius) {
            return terminal;
        }
    }
    return -1;
}

// 获取对应的电路模型元器件
CircuitSim::Component* GraphComponent::model() const {
    return s_circuit ? s_circuit->getComponent(m_id) : nullptr;
}

// 更新所有连接到此元器件的连线几何形状
void GraphComponent::updateConnectedWires() {
    for (WireItem* wire : m_wires) {
        if (wire) {
            wire->updateGeometry();
        }
    }
}

// 添加连线到此元器件
void GraphComponent::addWire(WireItem* wire) {
    if (wire && !m_wires.contains(wire)) {
        m_wires.push_back(wire);
    }
}

// 移除连线
void GraphComponent::removeWire(WireItem* wire) {
    m_wires.removeAll(wire);
}

// 绘制两个端口连接点（圆形）
void GraphComponent::paintConnectionPoints(QPainter* painter) {
    painter->setRenderHint(QPainter::Antialiasing, true);
    for (int terminal = 0; terminal < 2; ++terminal) {
        const QPointF pos = terminalLocalPos(terminal);
        // 悬停或选中时高亮显示
        painter->setPen(QPen(m_hovered || isSelected() ? QColor(60, 120, 255) : QColor(60, 60, 60), 2));
        painter->setBrush(m_hovered || isSelected() ? QColor(90, 160, 255) : QColor(70, 70, 70));
        painter->drawEllipse(pos, 4.5, 4.5);
    }
}

// 图形项变化事件处理：位置变化时更新模型坐标和连线
QVariant GraphComponent::itemChange(GraphicsItemChange change, const QVariant& value) {
    if (change == ItemPositionHasChanged) {
        if (auto* comp = model()) {
            comp->setPosition(pos().x(), pos().y());
        }
        updateConnectedWires();
    }
    return QGraphicsObject::itemChange(change, value);
}

// 鼠标悬停进入事件
void GraphComponent::hoverEnterEvent(QGraphicsSceneHoverEvent* event) {
    m_hovered = true;
    update();
    QGraphicsObject::hoverEnterEvent(event);
}

// 鼠标悬停离开事件
void GraphComponent::hoverLeaveEvent(QGraphicsSceneHoverEvent* event) {
    m_hovered = false;
    update();
    QGraphicsObject::hoverLeaveEvent(event);
}

// 鼠标双击事件：打开属性编辑对话框
void GraphComponent::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) {
    editProperties(QApplication::activeWindow());
    event->accept();
}

// 打开属性编辑对话框
void GraphComponent::editProperties(QWidget* parent) {
    ComponentPropertyDialog dialog(this, parent ? parent : QApplication::activeWindow());
    (void)dialog.exec();
}