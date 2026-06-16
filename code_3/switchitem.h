#ifndef SWITCHITEM_H
#define SWITCHITEM_H

#include "graphcomponent.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class SwitchItem : public GraphComponent {
public:
    bool m_isClosed; // 记录开关是闭合还是断开
    // 给亲爹补上 "Switch" 类型，同时初始化闭合状态
    SwitchItem(int id) : GraphComponent(id, "Switch"), m_isClosed(false) {}
    virtual QRectF boundingRect() const override {
        return QRectF(-25, -20, 50, 40);
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));

        // 画两个触点
        painter->drawEllipse(QRectF(-12, -3, 6, 6));
        painter->drawEllipse(QRectF(6, -3, 6, 6));

        // 画引脚
        painter->drawLine(-20, 0, -12, 0);
        painter->drawLine(12, 0, 20, 0);

        if (m_isClosed) {
            // 闭合状态：拨杆平放，连接两端
            painter->drawLine(-9, 0, 9, 0);
            painter->setPen(Qt::darkRed); // 闭合时标红提示
        } else {
            // 断开状态：拨杆翘起
            painter->drawLine(-9, 0, 8, -12);
            painter->setPen(Qt::darkBlue);
        }

        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-6, 18, "K");
    }

    // 🌟 监听鼠标双击事件，让开关动起来
    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        m_isClosed = !m_isClosed; // 状态反转
        update(); // 重新绘制，拨杆会瞬间落下/翘起
        QGraphicsItem::mouseDoubleClickEvent(event);
    }
};

#endif // SWITCHITEM_H