#ifndef CAPACITORITEM_H
#define CAPACITORITEM_H

#include "graphcomponent.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>

class CapacitorItem : public GraphComponent {
public:
    CapacitorItem(int id) : GraphComponent(id, "Capacitor") {}

    virtual QRectF boundingRect() const override {
        return QRectF(-40, -25, 80, 50);
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(isSelected() ? QBrush(QColor(200, 220, 255)) : Qt::white);

        painter->drawLine(-5, -12, -5, 12);
        painter->drawLine(5, -12, 5, 12);
        painter->drawLine(-40, 0, -5, 0);
        painter->drawLine(5, 0, 40, 0);

        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-6, -15, "C");

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

#endif // CAPACITORITEM_H