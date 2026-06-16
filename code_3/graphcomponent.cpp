#include "graphcomponent.h"

// 👇 引入必须要用到的鼠标库
#include <QCursor>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

// 1. 构造函数
GraphComponent::GraphComponent(int id, const QString& type)
    : m_id(id), m_type(type), m_voltage(0.0), m_current(0.0)
{
    // 开启基础拖拽和选中功能
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);

    // 👇 维度一：关键两步！开启鼠标悬停检测，并设置默认手型为“张开的小手”
    setAcceptHoverEvents(true);
    setCursor(Qt::OpenHandCursor);
}

// 2. 边界框
QRectF GraphComponent::boundingRect() const
{
    return QRectF(-40, -25, 80, 50);
}

// 3. 物理数据更新
void GraphComponent::updatePhysicsData(double voltage, double current)
{
    m_voltage = voltage;
    m_current = current;
    update();
}

// ==========================================
// 👇 维度一核心：具体的交互动作实现
// ==========================================

// 动作 A：鼠标悬停进来时 -> 放大 10%
void GraphComponent::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
    setScale(1.1);
    QGraphicsItem::hoverEnterEvent(event);
}

// 动作 B：鼠标离开时 -> 缩回原来的大小 (1.0)
void GraphComponent::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
    setScale(1.0);
    QGraphicsItem::hoverLeaveEvent(event);
}

// 动作 C：鼠标按下去拖拽时 -> 变成“抓紧的拳头”
void GraphComponent::mousePressEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(Qt::ClosedHandCursor);
    QGraphicsItem::mousePressEvent(event);
}

// 动作 D：鼠标松开时 -> 恢复“张开的小手”
void GraphComponent::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    setCursor(Qt::OpenHandCursor);
    QGraphicsItem::mouseReleaseEvent(event);
}