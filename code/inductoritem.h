#ifndef INDUCTORITEM_H
#define INDUCTORITEM_H

#include "graphcomponent.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class InductorItem : public GraphComponent {
public:
    // 给亲爹补上 "Inductor" 类型
    InductorItem(int id) : GraphComponent(id, "Inductor") {}

    virtual QRectF boundingRect() const override {
        return QRectF(-25, -15, 50, 30);
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));

        painter->drawArc(-15, -5, 10, 10, 0, 180 * 16);
        painter->drawArc(-5, -5, 10, 10, 0, 180 * 16);
        painter->drawArc(5, -5, 10, 10, 0, 180 * 16);

        painter->drawLine(-20, 0, -15, 0);
        painter->drawLine(15, 0, 20, 0);

        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-5, -8, "L");
    }
};

#endif // INDUCTORITEM_H