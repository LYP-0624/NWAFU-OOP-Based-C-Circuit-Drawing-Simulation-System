#include "circuitscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QSet>

#include "ComponentFactory.h"
#include "componentitems.h"
#include "graphcomponent.h"
#include "wireitem.h"

namespace {

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

CircuitScene::CircuitScene(QObject* parent)
    : QGraphicsScene(parent),
      m_circuit(nullptr),
      m_tool(Tool::Select),
      m_wireStartItem(nullptr),
      m_wireStartPort(-1),
      m_previewWire(nullptr) {
    setSceneRect(-700, -500, 1400, 1000);
}

void CircuitScene::setCircuit(CircuitSim::Circuit* circuit) {
    m_circuit = circuit;
    GraphComponent::setCircuit(circuit);
}

void CircuitScene::cancelPlacementMode() {
    cancelWireDrag();
    m_tool = Tool::Select;
}

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

GraphComponent* CircuitScene::componentItem(int componentId) const {
    auto it = m_componentItems.find(componentId);
    return it == m_componentItems.end() ? nullptr : it.value();
}

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

void CircuitScene::cancelWireDrag() {
    if (m_previewWire) {
        removeItem(m_previewWire);
        delete m_previewWire;
        m_previewWire = nullptr;
    }
    m_wireStartItem = nullptr;
    m_wireStartPort = -1;
}

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

void CircuitScene::removeComponentItem(GraphComponent* component) {
    if (!component) {
        return;
    }

    if (m_circuit) {
        m_circuit->removeComponent(component->id());
    }

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

void CircuitScene::mouseMoveEvent(QGraphicsSceneMouseEvent* event) {
    if (m_previewWire) {
        m_previewWire->setEndPoint(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void CircuitScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
    if (m_previewWire && event->button() == Qt::LeftButton) {
        finishWireDrag(event->scenePos());
        event->accept();
        return;
    }

    QGraphicsScene::mouseReleaseEvent(event);
}
