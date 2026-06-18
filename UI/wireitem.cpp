#include "wireitem.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

#include "graphcomponent.h"

WireItem::WireItem(GraphComponent* startItem, int startPort, GraphComponent* endItem, int endPort, int branchId)
    : m_startItem(startItem),
      m_endItem(endItem),
      m_startPort(startPort),
      m_endPort(endPort),
      m_branchId(branchId),
      m_temporary(false) {
    setZValue(-1.0);
    setPen(QPen(QColor(40, 40, 40), 2.0));
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setAcceptedMouseButtons(Qt::LeftButton);
    if (m_startItem) m_startItem->addWire(this);
    if (m_endItem) m_endItem->addWire(this);
    updateGeometry();
}

WireItem::WireItem(GraphComponent* startItem, int startPort, const QPointF& endPoint)
    : m_startItem(startItem),
      m_endItem(nullptr),
      m_endPoint(endPoint),
      m_startPort(startPort),
      m_endPort(1),
      m_branchId(-1),
      m_temporary(true) {
    setZValue(-1.0);
    setPen(QPen(QColor(90, 90, 90), 1.5, Qt::DashLine));
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    setAcceptedMouseButtons(Qt::NoButton);
    if (m_startItem) m_startItem->addWire(this);
    updateGeometry();
}

WireItem::~WireItem() {
    if (m_startItem) {
        m_startItem->removeWire(this);
    }
    if (m_endItem) {
        m_endItem->removeWire(this);
    }
}

void WireItem::setEndPoint(const QPointF& endPoint) {
    m_endPoint = endPoint;
    m_temporary = true;
    updateGeometry();
}

void WireItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    QPen pen = this->pen();
    if (isSelected()) {
        pen = QPen(QColor(220, 80, 40), 3.0);
    } else if (m_temporary) {
        pen = QPen(QColor(90, 90, 90), 1.5, Qt::DashLine);
    }
    setPen(pen);
    QGraphicsPathItem::paint(painter, option, widget);
}

void WireItem::updateGeometry() {
    prepareGeometryChange();

    const QPointF p1 = m_startItem ? m_startItem->terminalScenePos(m_startPort) : QPointF();
    const QPointF p2 = m_temporary || !m_endItem ? m_endPoint : m_endItem->terminalScenePos(m_endPort);

    QPointF control = (p1 + p2) * 0.5;
    const QPointF delta = p2 - p1;
    control += QPointF(-delta.y(), delta.x()) * 0.15;

    QPainterPath path;
    path.moveTo(p1);
    path.quadTo(control, p2);
    setPath(path);
}
