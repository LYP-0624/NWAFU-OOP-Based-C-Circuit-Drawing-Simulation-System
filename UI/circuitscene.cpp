#include "circuitscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QSet>

#include "ComponentFactory.h"
#include "componentitems.h"
#include "graphcomponent.h"
#include "wireitem.h"

namespace {

// 将工具类型枚举转换为元器件类型字符串
QString typeToSceneTool(CircuitScene::Tool tool) {
    switch (tool) {
        case CircuitScene::Tool::Resistor: return "Resistor";
        case CircuitScene::Tool::PowerSource: return "PowerSource";
        case CircuitScene::Tool::Bulb: return "Bulb";
        case CircuitScene::Tool::Ammeter: return "Ammeter";
        case CircuitScene::Tool::Voltmeter: return "Voltmeter";
        case CircuitScene::Tool::Capacitor: return "Capacitor";
        case CircuitScene::Tool::Inductor: return "Inductor";
        case CircuitScene::Tool::Switch: return "Switch";
        default: return "Select";
    }
}

} // namespace

// 构造函数：初始化场景、工具状态和连线预览
CircuitScene::CircuitScene(QObject* parent)
    : QGraphicsScene(parent),
      m_circuit(nullptr),
      m_tool(Tool::Select),
      m_wireStartItem(nullptr),
      m_wireStartPort(-1),
      m_previewWire(nullptr) {
    setSceneRect(-700, -500, 1400, 1000);  // 设置场景显示区域
}

// 绑定电路模型到场景
void CircuitScene::setCircuit(CircuitSim::Circuit* circuit) {
    m_circuit = circuit;
    GraphComponent::setCircuit(circuit);
}

// 取消当前放置模式，重置工具状态
void CircuitScene::cancelPlacementMode() {
    cancelWireDrag();
    m_tool = Tool::Select;
}

// 根据电路元器件创建对应的图形项
GraphComponent* CircuitScene::createComponentItem(CircuitSim::Component* component) {
    if (!component) {
        return nullptr;
    }

    GraphComponent* item = nullptr;
    switch (component->getType()) {
        case CircuitSim::ComponentType::RESISTOR:
            item = new ResistorItem(component->getId());
            break;
        case CircuitSim::ComponentType::POWER_SOURCE:
            item = new PowerSourceItem(component->getId());
            break;
        case CircuitSim::ComponentType::BULB:
            item = new LightBulbItem(component->getId());
            break;
        case CircuitSim::ComponentType::AMMETER:
            item = new AmmeterItem(component->getId());
            break;
        case CircuitSim::ComponentType::VOLTMETER:
            item = new VoltmeterItem(component->getId());
            break;
        case CircuitSim::ComponentType::CAPACITOR:
            item = new CapacitorItem(component->getId());
            break;
        case CircuitSim::ComponentType::INDUCTOR:
            item = new InductorItem(component->getId());
            break;
        case CircuitSim::ComponentType::SWITCH:
            item = new SwitchItem(component->getId());
            break;
    }

    if (item) {
        item->setPos(component->getX(), component->getY());
        addItem(item);
        m_componentItems.insert(component->getId(), item);
    }

    return item;
}

// 根据电路支路创建对应的连线图形项
void CircuitScene::createWireFromBranch(const CircuitSim::Branch* branch) {
    if (!branch || !branch->getStartComponent() || !branch->getEndComponent()) {
        return;
    }

    GraphComponent* startItem = componentItem(branch->getStartComponent()->getId());
    GraphComponent* endItem = componentItem(branch->getEndComponent()->getId());
    if (!startItem || !endItem) {
        return;
    }

    auto* wire = new WireItem(startItem, branch->getStartPortIndex(), endItem, branch->getEndPortIndex(), branch->getId());
    addItem(wire);
    m_wireItems.insert(branch->getId(), wire);
}

// 从电路模型同步所有图形项（清空后重建）
void CircuitScene::syncFromCircuit() {
    clear();
    m_componentItems.clear();
    m_wireItems.clear();
    m_wireStartItem = nullptr;
    m_wireStartPort = -1;
    m_previewWire = nullptr;

    if (!m_circuit) {
        return;
    }

    const auto& components = m_circuit->getComponents();
    for (const auto& [id, comp] : components) {
        createComponentItem(comp);
    }

    for (const auto* branch : m_circuit->getBranches()) {
        createWireFromBranch(branch);
    }
}

// 刷新所有图形项的显示状态
void CircuitScene::refreshVisuals() {
    for (auto* item : m_componentItems) {
        if (item) {
            item->update();
        }
    }
    for (auto* wire : m_wireItems) {
        if (wire) {
            wire->updateGeometry();
        }
    }
    update();
}

// 根据元器件ID获取对应的图形项
GraphComponent* CircuitScene::componentItem(int componentId) const {
    auto it = m_componentItems.find(componentId);
    return it == m_componentItems.end() ? nullptr : it.value();
}

// 获取指定位置处的元器件图形项及其端口索引
GraphComponent* CircuitScene::componentAt(const QPointF& scenePos, int* terminal) const {
    const QList<QGraphicsItem*> hits = items(scenePos);
    for (QGraphicsItem* raw : hits) {
        auto* comp = dynamic_cast<GraphComponent*>(raw);
        if (!comp) {
            continue;
        }
        if (terminal) {
            *terminal = comp->terminalAt(scenePos);
        }
        return comp;
    }
    return nullptr;
}

// 开始拖拽连线：记录起始元器件和端口，创建预览线
void CircuitScene::beginWireDrag(GraphComponent* component, int terminal, const QPointF& scenePos) {
    if (!component || terminal < 0) {
        return;
    }

    cancelWireDrag();
    m_wireStartItem = component;
    m_wireStartPort = terminal;
    m_previewWire = new WireItem(component, terminal, scenePos);
    addItem(m_previewWire);
}

// 结束拖拽连线：查找目标端口并创建实际支路连接
void CircuitScene::finishWireDrag(const QPointF& scenePos) {
    if (!m_wireStartItem || m_wireStartPort < 0) {
        cancelWireDrag();
        return;
    }

    int terminal = -1;
    GraphComponent* endItem = componentAt(scenePos, &terminal);
    if (!endItem || endItem == m_wireStartItem || terminal < 0 || !m_circuit) {
        cancelWireDrag();
        return;
    }

    CircuitSim::Component* startModel = m_circuit->getComponent(m_wireStartItem->id());
    CircuitSim::Component* endModel = m_circuit->getComponent(endItem->id());
    if (!startModel || !endModel) {
        cancelWireDrag();
        return;
    }

    CircuitSim::Branch* branch = m_circuit->connect(startModel->getPort(m_wireStartPort), endModel->getPort(terminal));
    if (branch) {
        auto* wire = new WireItem(m_wireStartItem, m_wireStartPort, endItem, terminal, branch->getId());
        addItem(wire);
        m_wireItems.insert(branch->getId(), wire);
    }

    cancelWireDrag();
}

// 取消当前连线拖拽，清除预览线
void CircuitScene::cancelWireDrag() {
    if (m_previewWire) {
        removeItem(m_previewWire);
        delete m_previewWire;
        m_previewWire = nullptr;
    }
    m_wireStartItem = nullptr;
    m_wireStartPort = -1;
}

// 在指定位置放置当前选中的元器件类型
void CircuitScene::placeComponentAt(const QPointF& scenePos) {
    if (!m_circuit) {
        return;
    }

    const QString type = typeToSceneTool(m_tool);
    if (type == "Select") {
        return;
    }

    CircuitSim::Component* component = CircuitSim::ComponentFactory::getInstance().create(
        type.toStdString(), m_circuit->getNextComponentId(), scenePos.x(), scenePos.y());
    if (!component) {
        return;
    }

    component->setPosition(scenePos.x(), scenePos.y());
    m_circuit->addComponent(component);
    createComponentItem(component);
    m_tool = Tool::Select;
}

// 删除所有选中的图形项（包括连线和元器件）
void CircuitScene::deleteSelectedItems() {
    const auto selected = selectedItems();
    QSet<int> selectedWireIds;
    QSet<int> selectedComponentIds;

    for (QGraphicsItem* raw : selected) {
        if (auto* wire = dynamic_cast<WireItem*>(raw)) {
            selectedWireIds.insert(wire->branchId());
        } else if (auto* comp = dynamic_cast<GraphComponent*>(raw)) {
            selectedComponentIds.insert(comp->id());
        }
    }

    for (int wireId : selectedWireIds) {
        if (auto* wire = m_wireItems.value(wireId, nullptr)) {
            removeWireItem(wire);
        }
    }

    for (int compId : selectedComponentIds) {
        if (auto* comp = m_componentItems.value(compId, nullptr)) {
            removeComponentItem(comp);
        }
    }
}

// 移除连线图形项并从电路模型删除对应支路
void CircuitScene::removeWireItem(WireItem* wire) {
    if (!wire) {
        return;
    }

    if (wire->branchId() >= 0 && m_circuit) {
        m_circuit->removeBranch(wire->branchId());
    }

    m_wireItems.remove(wire->branchId());
    removeItem(wire);
    delete wire;
}

// 移除元器件图形项并从电路模型删除，同时清理关联连线
void CircuitScene::removeComponentItem(GraphComponent* component) {
    if (!component) {
        return;
    }

    if (m_circuit) {
        m_circuit->removeComponent(component->id());
    }

    // 收集并删除与该元器件相连的所有连线
    QList<WireItem*> attached;
    for (auto* wire : m_wireItems) {
        if (wire && (wire->startItem() == component || wire->endItem() == component)) {
            attached.push_back(wire);
        }
    }
    for (WireItem* wire : attached) {
        removeWireItem(wire);
    }

    m_componentItems.remove(component->id());
    removeItem(component);
    delete component;
}

// 鼠标按下事件处理：右键取消，左键开始连线或放置元器件
void CircuitScene::mousePressEvent(QGraphicsSceneMouseEvent* event) {
    if (event->button() == Qt::RightButton) {
        cancelPlacementMode();
        event->accept();
        return;
    }

    if (event->button() == Qt::LeftButton) {
        int terminal = -1;
        GraphComponent* comp = componentAt(event->scenePos(), &terminal);
        if (comp && terminal >= 0) {
            beginWireDrag(comp, terminal, event->scenePos());
            event->accept();
            return;
        }

        if (m_tool != Tool::Select) {
            placeComponentAt(event->scenePos());
            event->accept();
            return;
        }
    }

    QGraphicsScene::mousePressEvent(event);
}

// 鼠标移动事件处理：更新连线预览终点位置
void CircuitScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_previewWire) {
        m_previewWire->setEndPoint(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

// 鼠标释放事件处理：左键释放时完成连线
void CircuitScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (m_previewWire && event->button() == Qt::LeftButton) {
        finishWireDrag(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}