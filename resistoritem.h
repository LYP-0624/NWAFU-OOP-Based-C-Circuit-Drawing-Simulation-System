#ifndef RESISTORITEM_H
#define RESISTORITEM_H

#include "graphcomponent.h"
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QFont>

// 1. 电阻前端图元
class ResistorItem : public GraphComponent {
public:
    ResistorItem(int id) : GraphComponent(id, "Resistor") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(isSelected() ? QBrush(QColor(200, 220, 255)) : Qt::white);
        painter->drawRect(-25, -10, 50, 20);
        painter->drawLine(-40, 0, -25, 0);
        painter->drawLine(25, 0, 40, 0);
        painter->setPen(Qt::darkBlue);
        painter->drawText(-25, -15, QString("R%1: %2V").arg(m_id).arg(m_voltage));
    }
};

// 2. 直流电源前端图元
class PowerSourceItem : public GraphComponent {
public:
    PowerSourceItem(int id) : GraphComponent(id, "PowerSource") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(-40, 0, -5, 0);
        painter->drawLine(5, 0, 40, 0);
        painter->drawLine(-5, -20, -5, 20);
        painter->setPen(QPen(Qt::black, 4));
        painter->drawLine(5, -10, 5, 10);
        painter->setPen(Qt::darkRed);
        painter->drawText(-20, -25, QString("E%1: %2V").arg(m_id).arg(m_voltage));
    }
};

// 3. 小灯泡前端图元
class LightBulbItem : public GraphComponent {
public:
    LightBulbItem(int id) : GraphComponent(id, "LightBulb") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(m_voltage > 0.1 ? QBrush(Qt::yellow) : Qt::white);
        painter->drawEllipse(-15, -15, 30, 30);
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);
        painter->drawLine(-10, -10, 10, 10);
        painter->drawLine(-10, 10, 10, -10);
        painter->setPen(Qt::darkGreen);
        painter->drawText(-20, -20, QString("L%1").arg(m_id));
    }
};

// 4. 电流表前端图元
class AmmeterItem : public GraphComponent {
public:
    AmmeterItem(int id) : GraphComponent(id, "Ammeter") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(Qt::white);
        painter->drawEllipse(-15, -15, 30, 30);
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);
        painter->setFont(QFont("Arial", 12, QFont::Bold));
        painter->drawText(-7, 5, "A");
        painter->setFont(QFont("Arial", 9));
        painter->drawText(-20, -20, QString("%1A").arg(m_current));
    }
};

// 5. 电压表前端图元
class VoltmeterItem : public GraphComponent {
public:
    VoltmeterItem(int id) : GraphComponent(id, "Voltmeter") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option); Q_UNUSED(widget);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(Qt::white);
        painter->drawEllipse(-15, -15, 30, 30);
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);
        painter->setFont(QFont("Arial", 12, QFont::Bold));
        painter->drawText(-7, 5, "V");
        painter->setFont(QFont("Arial", 9));
        painter->drawText(-20, -20, QString("%1V").arg(m_voltage));
    }
};






class WireItem : public QGraphicsItem {
public:
    GraphComponent* m_startItem;
    GraphComponent* m_endItem;

    // 构造函数：只接收两个元件
    WireItem(GraphComponent* start, GraphComponent* end)
        : m_startItem(start), m_endItem(end) {
        setZValue(-1); // 压在最下面
    }

    // 暴力包围盒
    QRectF boundingRect() const override {
        return QRectF(-9999, -9999, 19998, 19998);
    }

    // 绘制逻辑
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        // 如果起点或终点丢了，直接不画
        if (!m_startItem || !m_endItem) return;

        // 🚨 安全获取中心点：使用 scenePos() 和 boundingRect().center()
        // 这样写最稳，不需要编译器去深入解析太复杂的内部结构
        QPointF p1 = m_startItem->scenePos() + m_startItem->boundingRect().center();
        QPointF p2 = m_endItem->scenePos() + m_endItem->boundingRect().center();

        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));

        // 转换回导线自身坐标并绘制
        painter->drawLine(mapFromScene(p1), mapFromScene(p2));
    }
};

#endif // RESISTORITEM_H