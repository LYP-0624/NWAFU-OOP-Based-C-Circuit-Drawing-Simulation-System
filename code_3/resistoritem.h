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
        Q_UNUSED(option);
        Q_UNUSED(widget);

        // 开启抗锯齿，让线条更平滑
        painter->setRenderHint(QPainter::Antialiasing);

        // 设置边框为黑色，粗细为2
        painter->setPen(QPen(Qt::black, 2));

        // 选中时变成淡蓝色，平时是纯白色
        painter->setBrush(isSelected() ? QBrush(QColor(200, 220, 255)) : Qt::white);

        // 画一个居中的国标/欧标长方形（宽40，高20）
        painter->drawRect(-20, -10, 40, 20);

        // 在中心画一个专业的字母 R
        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-6, 5, "R");

        // 如果需要显示头顶的电压/编号，可以保留这行
        painter->setPen(Qt::darkGreen);
        painter->setFont(QFont("Arial", 8));
        painter->drawText(-20, -15, QString("R%1: %2V").arg(m_id).arg(m_voltage));
    }
};

// 2. 直流电源前端图元
class PowerSourceItem : public GraphComponent {
public:
    PowerSourceItem(int id) : GraphComponent(id, "PowerSource") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        painter->setPen(QPen(Qt::black, 2));

        // 【已删除外延引脚，只保留一长一短竖线】
        painter->drawLine(-10, -15, -10, 15); // 长线 (正极)
        painter->drawLine(10, -8, 10, 8);     // 短线 (负极)
    }
};

// 3. 小灯泡前端图元
class LightBulbItem : public GraphComponent {
public:
    LightBulbItem(int id) : GraphComponent(id, "LightBulb") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        // 【已删除引脚，加入通电变黄效果】
        if (m_voltage > 0) {
            painter->setBrush(Qt::yellow); // 通电发黄光
        } else {
            painter->setBrush(Qt::NoBrush);
        }

        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(-15, -15, 30, 30);

        // 画里面的叉号
        painter->drawLine(-10, -10, 10, 10);
        painter->drawLine(-10, 10, 10, -10);
    }
};

// 4. 电流表前端图元
class AmmeterItem : public GraphComponent {
public:
    AmmeterItem(int id) : GraphComponent(id, "Ammeter") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        painter->setPen(QPen(Qt::black, 2));

        // 【已删除引脚，只保留表盘和数据】
        painter->drawEllipse(-15, -15, 30, 30);
        painter->drawText(-15, -15, 30, 30, Qt::AlignCenter, "A");

        // 如果通电了，显示电流值
        if (m_voltage > 0 || m_current > 0) {
            painter->setPen(Qt::blue);
            painter->drawText(-30, 20, 60, 20, Qt::AlignCenter, QString("%1 A").arg(m_current));
        }
    }
};

// 5. 电压表前端图元
class VoltmeterItem : public GraphComponent {
public:
    VoltmeterItem(int id) : GraphComponent(id, "Voltmeter") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        painter->setPen(QPen(Qt::black, 2));

        // 【已删除引脚，只保留表盘和数据】
        painter->drawEllipse(-15, -15, 30, 30);
        painter->drawText(-15, -15, 30, 30, Qt::AlignCenter, "V");

        if (m_voltage > 0 || m_current > 0) {
            painter->setPen(Qt::red);
            painter->drawText(-30, 20, 60, 20, Qt::AlignCenter, QString("%1 V").arg(m_voltage));
        }
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
        setFlag(QGraphicsItem::ItemIsSelectable, true);
    }

    // 暴力包围盒
    QRectF boundingRect() const override {
        return QRectF(-5, -5, 5, 5);
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