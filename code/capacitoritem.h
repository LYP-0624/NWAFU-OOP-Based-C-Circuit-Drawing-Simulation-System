#ifndef CAPACITORITEM_H
#define CAPACITORITEM_H


#include "graphcomponent.h" // 确保这里是你元件的通用基类头文件
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class CapacitorItem : public GraphComponent {
public:
    // 给亲爹补上 "Capacitor" 类型
    CapacitorItem(int id) : GraphComponent(id, "Capacitor") {}

    // 规定元件的点击和碰撞范围
    virtual QRectF boundingRect() const override {
        return QRectF(-25, -25, 50, 50);
    }

    // 我们之前写好的完美画法
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(isSelected() ? QBrush(QColor(200, 220, 255)) : Qt::white);

        painter->drawLine(-5, -12, -5, 12);
        painter->drawLine(5, -12, 5, 12);

        painter->drawLine(-20, 0, -5, 0);
        painter->drawLine(5, 0, 20, 0);

        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-6, -15, "C");
    }
};

#endif // CAPACITORITEM_H