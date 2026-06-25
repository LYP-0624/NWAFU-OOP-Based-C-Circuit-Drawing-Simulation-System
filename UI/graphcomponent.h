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

// 图形元件基类
class GraphComponent : public QGraphicsObject {
public:
    GraphComponent(int id, const QString& type, QGraphicsItem* parent = nullptr);

    static CircuitSim::Circuit* s_circuit;  // 静态电路引用
    static void setCircuit(CircuitSim::Circuit* c);

    int id() const { return m_id; }
    QString typeName() const { return m_type; }

    QRectF boundingRect() const override;       // 返回元件包围区域
    QPointF terminalLocalPos(int terminal) const; // 获取端子局部坐标
    QPointF terminalScenePos(int terminal) const; // 获取端子场景坐标
    int terminalAt(const QPointF& scenePos, qreal radius = 9.0) const; // 检测点击的端子

    CircuitSim::Component* model() const;      // 获取对应的电路元件
    void updateConnectedWires();               // 更新连接的导线
    void addWire(WireItem* wire);              // 添加导线
    void removeWire(WireItem* wire);           // 移除导线

    void paintConnectionPoints(QPainter* painter); // 绘制连接点

    virtual void editProperties(QWidget* parent = nullptr); // 编辑属性

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

    int m_id;                    // 元件ID
    QString m_type;              // 元件类型
    bool m_hovered;              // 悬停状态
    QList<WireItem*> m_wires;    // 连接的导线列表
};

#endif // GRAPHCOMPONENT_H
