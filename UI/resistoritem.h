#ifndef RESISTORITEM_H
#define RESISTORITEM_H

#include "graphcomponent.h"
#include <QPen>
#include <QBrush>
#include <QColor>
#include <QFont>
#include <QPainterPath>
#include <cmath>

class ResistorItem : public GraphComponent {
public:
    ResistorItem(int id) : GraphComponent(id, "Resistor") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(isSelected() ? QBrush(QColor(200, 220, 255)) : Qt::white);

        painter->drawRect(-20, -10, 40, 20);

        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-6, 5, "R");

        painter->setPen(Qt::darkGreen);
        painter->setFont(QFont("Arial", 8));
        painter->drawText(-20, -15, QString("R%1: %2V").arg(m_id).arg(m_voltage));

        drawPins(painter);
    }

private:
    void drawPins(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(-40, 0, -20, 0);
        painter->drawLine(20, 0, 40, 0);

        QPointF lp = getLeftConnectionPoint();
        QPointF rp = getRightConnectionPoint();
        painter->setBrush(Qt::white);
        painter->drawEllipse(lp, 3, 3);
        painter->drawEllipse(rp, 3, 3);
    }
};

class PowerSourceItem : public GraphComponent {
public:
    PowerSourceItem(int id) : GraphComponent(id, "PowerSource") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        painter->setPen(QPen(Qt::black, 2));
        QLineF longLine(-10, -15, -10, 15);
        QLineF shortLine(10, -8, 10, 8);
        painter->drawLine(longLine);
        painter->drawLine(shortLine);

        painter->setPen(Qt::red);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-16, -20, "+");

        painter->setPen(Qt::blue);
        painter->drawText(4, -20, "-");

        painter->setPen(Qt::darkGreen);
        painter->setFont(QFont("Arial", 7));
        painter->drawText(-20, 22, QString("%1V").arg(m_voltage));

        drawPins(painter);
    }

private:
    void drawPins(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);

        QPointF lp = getLeftConnectionPoint();
        QPointF rp = getRightConnectionPoint();
        painter->setBrush(Qt::white);
        painter->drawEllipse(lp, 3, 3);
        painter->drawEllipse(rp, 3, 3);
    }
};

class LightBulbItem : public GraphComponent {
public:
    bool m_isLit;
    LightBulbItem(int id) : GraphComponent(id, "LightBulb"), m_isLit(false) {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        if (m_isLit) {
            painter->setBrush(QColor(255, 255, 100));
        } else {
            painter->setBrush(Qt::white);
        }

        painter->setPen(QPen(Qt::black, 2));
        painter->drawEllipse(-15, -15, 30, 30);

        painter->drawLine(-10, -10, 10, 10);
        painter->drawLine(-10, 10, 10, -10);

        drawPins(painter);
    }

    void updatePhysicsData(double voltage, double current) {
        m_voltage = voltage;
        m_current = current;
        if (std::abs(voltage) < 1e-6) {
            m_isLit = false;
        }
        update();
    }

private:
    void drawPins(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);

        QPointF lp = getLeftConnectionPoint();
        QPointF rp = getRightConnectionPoint();
        painter->setBrush(Qt::white);
        painter->drawEllipse(lp, 3, 3);
        painter->drawEllipse(rp, 3, 3);
    }
};

class AmmeterItem : public GraphComponent {
public:
    AmmeterItem(int id) : GraphComponent(id, "Ammeter") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(isSelected() ? QBrush(QColor(200, 220, 255)) : Qt::white);
        painter->drawEllipse(-15, -15, 30, 30);

        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-15, -15, 30, 30, Qt::AlignCenter, "A");

        if (m_voltage > 0 || m_current > 0) {
            painter->setPen(Qt::blue);
            painter->setFont(QFont("Arial", 7));
            painter->drawText(-30, 20, 60, 20, Qt::AlignCenter, QString("%1A").arg(m_current, 0, 'f', 3));
        }

        drawPins(painter);
    }

private:
    void drawPins(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);

        QPointF lp = getLeftConnectionPoint();
        QPointF rp = getRightConnectionPoint();
        painter->setBrush(Qt::white);
        painter->drawEllipse(lp, 3, 3);
        painter->drawEllipse(rp, 3, 3);
    }
};

class VoltmeterItem : public GraphComponent {
public:
    VoltmeterItem(int id) : GraphComponent(id, "Voltmeter") {}
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        painter->setRenderHint(QPainter::Antialiasing);

        painter->setPen(QPen(Qt::black, 2));
        painter->setBrush(isSelected() ? QBrush(QColor(200, 220, 255)) : Qt::white);
        painter->drawEllipse(-15, -15, 30, 30);

        painter->setPen(Qt::darkRed);
        painter->setFont(QFont("Arial", 10, QFont::Bold));
        painter->drawText(-15, -15, 30, 30, Qt::AlignCenter, "V");

        if (m_voltage > 0 || m_current > 0) {
            painter->setPen(Qt::red);
            painter->setFont(QFont("Arial", 7));
            painter->drawText(-30, 20, 60, 20, Qt::AlignCenter, QString("%1V").arg(m_voltage, 0, 'f', 2));
        }

        drawPins(painter);
    }

private:
    void drawPins(QPainter* painter) {
        painter->setPen(QPen(Qt::black, 2));
        painter->drawLine(-40, 0, -15, 0);
        painter->drawLine(15, 0, 40, 0);

        QPointF lp = getLeftConnectionPoint();
        QPointF rp = getRightConnectionPoint();
        painter->setBrush(Qt::white);
        painter->drawEllipse(lp, 3, 3);
        painter->drawEllipse(rp, 3, 3);
    }
};

class WireItem : public QGraphicsItem {
public:
    GraphComponent* m_startItem;
    GraphComponent* m_endItem;
    int m_startPort;
    int m_endPort;

    WireItem(GraphComponent* start, GraphComponent* end)
        : m_startItem(start), m_endItem(end), m_startPort(0), m_endPort(0) {
        setZValue(-1);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
        computeBestPorts();
    }

    WireItem(GraphComponent* start, GraphComponent* end, int startPort, int endPort)
        : m_startItem(start), m_endItem(end), m_startPort(startPort), m_endPort(endPort) {
        setZValue(-1);
        setFlag(QGraphicsItem::ItemIsSelectable, true);
    }

    QRectF boundingRect() const override {
        if (!m_startItem || !m_endItem) {
            return QRectF(-10, -10, 20, 20);
        }
        QPointF p1 = m_startItem->getConnectionPointScene(m_startPort);
        QPointF p2 = m_endItem->getConnectionPointScene(m_endPort);
        QPointF mid = (p1 + p2) / 2.0;
        QPointF dir = p2 - p1;
        double dist = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
        QPointF perp(-dir.y(), dir.x());
        if (dist > 1e-6) {
            perp = perp * (0.3 * dist) / dist;
        }
        QPointF cp = mid + perp;

        QRectF rect = QRectF(p1, p2).normalized();
        rect = rect.united(QRectF(cp, cp));
        rect.adjust(-10, -10, 10, 10);
        return mapFromScene(rect).boundingRect();
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override {
        Q_UNUSED(option);
        Q_UNUSED(widget);
        if (!m_startItem || !m_endItem) return;

        QPointF p1 = m_startItem->getConnectionPointScene(m_startPort);
        QPointF p2 = m_endItem->getConnectionPointScene(m_endPort);

        painter->setRenderHint(QPainter::Antialiasing);
        if (isSelected()) {
            painter->setPen(QPen(QColor(100, 150, 255), 3));
        } else {
            painter->setPen(QPen(Qt::black, 2));
        }

        QPointF lp1 = mapFromScene(p1);
        QPointF lp2 = mapFromScene(p2);

        QPointF mid = (lp1 + lp2) / 2.0;
        QPointF dir = lp2 - lp1;
        double dist = std::sqrt(dir.x() * dir.x() + dir.y() * dir.y());
        if (dist < 1e-6) {
            painter->drawLine(lp1, lp2);
            return;
        }

        QPointF perp(-dir.y(), dir.x());
        perp = perp * (0.3 * dist) / dist;
        QPointF cp = mid + perp;

        QPainterPath path;
        path.moveTo(lp1);
        path.quadTo(cp, lp2);
        painter->drawPath(path);
    }

private:
    void computeBestPorts() {
        if (!m_startItem || !m_endItem) return;

        double bestDist = 1e9;
        int bestSP = 0, bestEP = 0;

        for (int sp = 0; sp <= 1; sp++) {
            for (int ep = 0; ep <= 1; ep++) {
                QPointF p1 = m_startItem->getConnectionPointScene(sp);
                QPointF p2 = m_endItem->getConnectionPointScene(ep);
                double dx = p1.x() - p2.x();
                double dy = p1.y() - p2.y();
                double dist = dx * dx + dy * dy;
                if (dist < bestDist) {
                    bestDist = dist;
                    bestSP = sp;
                    bestEP = ep;
                }
            }
        }

        m_startPort = bestSP;
        m_endPort = bestEP;
    }
};

#endif // RESISTORITEM_H