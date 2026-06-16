#ifndef GRAPHCOMPONENT_H
#define GRAPHCOMPONENT_H

#include <QGraphicsItem>
#include <QString>

// 👇 提前声明我们要用到的鼠标事件类型
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;

class GraphComponent : public QGraphicsItem {
public:
    int m_id;
    QString m_type;
    double m_voltage = 0.0;
    double m_current = 0.0;

    // 构造函数
    GraphComponent(int id, const QString& type);

    // 规定元件的物理边界框
    virtual QRectF boundingRect() const override;

    // 预留给后端的更新接口
    void updatePhysicsData(double voltage, double current);

    // 纯虚函数：留给子类去画具体的长相
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;

protected:
    // ==========================================
    // 👇 维度一核心：在这里“声明”四大鼠标交互事件
    // ==========================================
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
};

#endif // GRAPHCOMPONENT_H