#ifndef CIRCUITSCENE_H
#define CIRCUITSCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPointF>

#include "Circuit.h"

class QGraphicsSceneMouseEvent;
class GraphComponent;
class WireItem;

class CircuitScene : public QGraphicsScene {
public:
    enum class Tool {
        Select,
        Resistor,
        PowerSource,
        Bulb,
        Ammeter,
        Voltmeter,
        Capacitor,
        Inductor,
        Switch
    };

    explicit CircuitScene(QObject* parent = nullptr);

    void setCircuit(CircuitSim::Circuit* circuit);
    CircuitSim::Circuit* circuit() const { return m_circuit; }

    void setTool(Tool tool) { m_tool = tool; }
    Tool tool() const { return m_tool; }

    void syncFromCircuit();
    void refreshVisuals();
    void deleteSelectedItems();
    void cancelPlacementMode();

    GraphComponent* componentItem(int componentId) const;

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    CircuitSim::Circuit* m_circuit;
    Tool m_tool;
    GraphComponent* m_wireStartItem;
    int m_wireStartPort;
    WireItem* m_previewWire;
    QMap<int, GraphComponent*> m_componentItems;
    QMap<int, WireItem*> m_wireItems;

    GraphComponent* createComponentItem(CircuitSim::Component* component);
    void createWireFromBranch(const CircuitSim::Branch* branch);
    void removeWireItem(WireItem* wire);
    void removeComponentItem(GraphComponent* component);
    void beginWireDrag(GraphComponent* component, int terminal, const QPointF& scenePos);
    void finishWireDrag(const QPointF& scenePos);
    void cancelWireDrag();
    void placeComponentAt(const QPointF& scenePos);
    GraphComponent* componentAt(const QPointF& scenePos, int* terminal = nullptr) const;
};

#endif // CIRCUITSCENE_H
