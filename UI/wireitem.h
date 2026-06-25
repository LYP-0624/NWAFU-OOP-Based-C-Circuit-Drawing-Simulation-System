#ifndef WIREITEM_H
#define WIREITEM_H

#include <QGraphicsPathItem>
#include <QPointF>

class GraphComponent;

// 导线图形项
class WireItem : public QGraphicsPathItem {
public:
    WireItem(GraphComponent* startItem, int startPort, GraphComponent* endItem, int endPort, int branchId = -1);
    WireItem(GraphComponent* startItem, int startPort, const QPointF& endPoint);
    ~WireItem() override;

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override; // 绘制导线
    void setEndPoint(const QPointF& endPoint);    // 设置终点(临时导线)
    void updateGeometry();                        // 更新几何形状

    GraphComponent* startItem() const { return m_startItem; }
    GraphComponent* endItem() const { return m_endItem; }
    int startPort() const { return m_startPort; }
    int endPort() const { return m_endPort; }
    int branchId() const { return m_branchId; }

private:
    GraphComponent* m_startItem;  // 起点元件
    GraphComponent* m_endItem;    // 终点元件
    QPointF m_endPoint;           // 临时终点
    int m_startPort;              // 起点端口
    int m_endPort;                // 终点端口
    int m_branchId;               // 支路ID
    bool m_temporary;             // 是否为临时导线
};

#endif // WIREITEM_H
