#ifndef GRAPHCOMPONENT_H
#define GRAPHCOMPONENT_H

#include <QGraphicsObject>
#include <QList>
#include <QPointF>
#include <QString>
#include <QVariant>

#include "Circuit.h"

class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;
class QPainter;
class QVariant;
class WireItem;

class GraphComponent : public QGraphicsObject {
public:
    GraphComponent(int id, const QString& type, QGraphicsItem* parent = nullptr);

    static CircuitSim::Circuit* s_circuit;
    static void setCircuit(CircuitSim::Circuit* c);

    int id() const { return m_id; }
    QString typeName() const { return m_type; }

    QRectF boundingRect() const override;
    QPointF terminalLocalPos(int terminal) const;
    QPointF terminalScenePos(int terminal) const;
    int terminalAt(const QPointF& scenePos, qreal radius = 9.0) const;

    CircuitSim::Component* model() const;
    void updateConnectedWires();
    void addWire(WireItem* wire);
    void removeWire(WireItem* wire);

    void paintConnectionPoints(QPainter* painter);

    virtual void editProperties(QWidget* parent = nullptr);

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    int m_id;
    QString m_type;
    bool m_hovered;
    QList<WireItem*> m_wires;
};

#endif // GRAPHCOMPONENT_H
