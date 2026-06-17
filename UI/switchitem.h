#ifndef SWITCHITEM_H
#define SWITCHITEM_H

#include "graphcomponent.h"
#include <QPainter>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

class SwitchItem : public GraphComponent {
public:
    bool m_isClosed;
    SwitchItem(int id) : GraphComponent(id, "Switch"), m_isClosed(false) {}

    virtual QRectF boundingRect() const override {
        return QRectF(-40, -25, 80, 50);
    }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);
        painter->setPen(QPen(Qt::black, 2));

        painter->drawEllipse(QRectF(-12, -3, 6, 6));
        painter->drawEllipse(QRectF(6, -3, 6, 6));
        painter->drawLine(-40, 0, -12, 0);
        painter->drawLine(12, 0, 40, 0);

        if (m_isClosed) {
            painter->drawLine(-9, 0, 9, 0);
            painter->setPen(Qt::darkRed);
        } else {
            painter->drawLine(-9, 0, 8, -12);
            painter->setPen(Qt::darkBlue);
        }

        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-6, 18, "K");

        drawPins(painter);
    }

    virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override {
        m_isClosed = !m_isClosed;
        update();
        QGraphicsItem::mouseDoubleClickEvent(event);
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

#endif // SWITCHITEM_H