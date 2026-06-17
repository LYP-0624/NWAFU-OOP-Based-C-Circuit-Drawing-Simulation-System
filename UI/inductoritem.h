#ifndef INDUCTORITEM_H
#define INDUCTORITEM_H

#include "graphcomponent.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class InductorItem : public GraphComponent {
public:
    InductorItem(int id) : GraphComponent(id, "Inductor") {}

    virtual QRectF boundingRect() const override {
        return QRectF(-40, -25, 80, 50);
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));

        painter->drawArc(-15, -5, 10, 10, 0, 180 * 16);
        painter->drawArc(-5, -5, 10, 10, 0, 180 * 16);
        painter->drawArc(5, -5, 10, 10, 0, 180 * 16);
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);

        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-5, -8, "L");

        painter->setPen(Qt::darkGreen);
        painter->setFont(QFont("Arial", 7));
        painter->drawText(-20, 20, QString("%1V").arg(m_voltage));

        drawPins(painter);
    }

private:
    void drawPins(QPainter* painter) {
        QPointF lp = getLeftConnectionPoint();
        QPointF rp = getRightConnectionPoint();
        painter->setBrush(Qt::white);
        painter->drawEllipse(lp, 3, 3);
        painter->drawEllipse(rp, 3, 3);
    }
};

#endif // INDUCTORITEM_H