#ifndef WIREITEM_H
#define WIREITEM_H

#include <QGraphicsPathItem>
#include <QPointF>

class GraphComponent;

class WireItem : public QGraphicsPathItem {
public:
    WireItem(GraphComponent* startItem, int startPort, GraphComponent* endItem, int endPort, int branchId = -1);
    WireItem(GraphComponent* startItem, int startPort, const QPointF& endPoint);
    ~WireItem() override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void setEndPoint(const QPointF& endPoint);
    void updateGeometry();

    GraphComponent* startItem() const { return m_startItem; }
    GraphComponent* endItem() const { return m_endItem; }
    int startPort() const { return m_startPort; }
    int endPort() const { return m_endPort; }
    int branchId() const { return m_branchId; }

private:
    GraphComponent* m_startItem;
    GraphComponent* m_endItem;
    QPointF m_endPoint;
    int m_startPort;
    int m_endPort;
    int m_branchId;
    bool m_temporary;
};

#endif // WIREITEM_H
