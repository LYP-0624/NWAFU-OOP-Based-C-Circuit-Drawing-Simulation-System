#include "graphcomponent.h"

GraphComponent::GraphComponent(int id, const QString& type)
    : m_id(id), m_type(type), m_voltage(0.0), m_current(0.0), m_selectedPort(0)
{
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

QRectF GraphComponent::boundingRect() const
{
    return QRectF(-40, -25, 80, 50);
}

void GraphComponent::updatePhysicsData(double voltage, double current)
{
    m_voltage = voltage;
    m_current = current;
    update();
}

QPointF GraphComponent::getLeftConnectionPoint() const
{
    return QPointF(-40, 0);
}

QPointF GraphComponent::getRightConnectionPoint() const
{
    return QPointF(40, 0);
}

QPointF GraphComponent::getConnectionPointScene(int port) const
{
    QPointF localPoint = (port == 0) ? getLeftConnectionPoint() : getRightConnectionPoint();
    return scenePos() + localPoint;
}

void GraphComponent::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = event->pos();

    m_selectedPort = (pos.x() < 0) ? 0 : 1;

    QGraphicsItem::mousePressEvent(event);
}