#ifndef GROUNDITEM_H
#define GROUNDITEM_H

#include "graphcomponent.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class GroundItem : public GraphComponent {
public:
    GroundItem(int id) : GraphComponent(id, "Ground") {}

    virtual QRectF boundingRect() const override {
        return QRectF(-40, -25, 80, 50);
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));

        painter->drawLine(0, -20, 0, 0);
        painter->drawLine(-12, 0, 12, 0);
        painter->drawLine(-8, 6, 8, 6);
        painter->drawLine(-4, 12, 4, 12);

        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(15, 5, "GND");

        drawPins(painter);
    }

private:
    void drawPins(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(-40, 0, 0, -20);

        QPointF lp = getLeftConnectionPoint();
        QPointF rp = getRightConnectionPoint();
        painter->setBrush(Qt::white);
        painter->drawEllipse(lp, 3, 3);
        painter->drawEllipse(rp, 3, 3);
    }
};

#endif // GROUNDITEM_H