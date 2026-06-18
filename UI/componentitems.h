#ifndef COMPONENTITEMS_H
#define COMPONENTITEMS_H

#include <QFont>
#include <QPainter>
#include <QRectF>
#include <QStyleOptionGraphicsItem>

#include "Ammeter.h"
#include "Bulb.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "PowerSource.h"
#include "Resistor.h"
#include "Switch.h"
#include "Voltmeter.h"
#include "graphcomponent.h"

namespace {

inline QString formatResistance(double ohms) {
    if (ohms >= 1000000.0) return QString("%1 MOhm").arg(ohms / 1000000.0, 0, 'f', 1);
    if (ohms >= 1000.0) return QString("%1 kOhm").arg(ohms / 1000.0, 0, 'f', 1);
    return QString("%1 Ohm").arg(ohms, 0, 'f', 0);
}

inline QString formatVoltage(double volts) {
    return QString("%1 V").arg(volts, 0, 'f', 1);
}

inline QString formatCurrent(double amps) {
    return QString("%1 A").arg(amps, 0, 'f', 3);
}

inline QString formatCapacitance(double farads) {
    if (farads >= 1e-3) return QString("%1 mF").arg(farads * 1000.0, 0, 'f', 3);
    if (farads >= 1e-6) return QString("%1 uF").arg(farads * 1e6, 0, 'f', 3);
    return QString("%1 F").arg(farads, 0, 'g', 3);
}

inline QString formatInductance(double henry) {
    if (henry >= 1.0) return QString("%1 H").arg(henry, 0, 'f', 3);
    if (henry >= 1e-3) return QString("%1 mH").arg(henry * 1000.0, 0, 'f', 3);
    return QString("%1 uH").arg(henry * 1e6, 0, 'f', 3);
}

inline void drawCaptionBlock(QPainter* painter, const QString& title, const QString& value, const QColor& valueColor) {
    painter->save();
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setPen(Qt::black);
    painter->setFont(QFont("Arial", 8, QFont::Bold));
    painter->drawText(QRectF(-50, 30, 100, 16), Qt::AlignCenter, title);
    painter->setPen(valueColor);
    painter->setFont(QFont("Arial", 8));
    painter->drawText(QRectF(-50, 44, 100, 16), Qt::AlignCenter, value);
    painter->restore();
}

inline void drawLead(QPainter* painter, qreal fromX, qreal toX) {
    painter->drawLine(QPointF(fromX, 0), QPointF(toX, 0));
}

} // namespace

class ResistorItem : public GraphComponent {
public:
    explicit ResistorItem(int id) : GraphComponent(id, "Resistor") {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        drawLead(painter, terminalLocalPos(0).x(), -24);
        drawLead(painter, 24, terminalLocalPos(1).x());

        const QPointF pts[] = {
            QPointF(-24, 0), QPointF(-18, -8), QPointF(-12, 8), QPointF(-6, -8),
            QPointF(0, 8), QPointF(6, -8), QPointF(12, 8), QPointF(18, -8), QPointF(24, 0)
        };
        const int pointCount = static_cast<int>(sizeof(pts) / sizeof(pts[0]));
        for (int i = 0; i + 1 < pointCount; ++i) {
            painter->drawLine(pts[i], pts[i + 1]);
        }

        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double r = props.count("resistance") ? props.at("resistance") : 100.0;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatResistance(r), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

class PowerSourceItem : public GraphComponent {
public:
    explicit PowerSourceItem(int id) : GraphComponent(id, "PowerSource") {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        drawLead(painter, terminalLocalPos(0).x(), -18);
        drawLead(painter, 18, terminalLocalPos(1).x());

        painter->drawLine(QPointF(-10, -14), QPointF(-10, 14));
        painter->drawLine(QPointF(6, -8), QPointF(6, 8));

        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double value = props.count("value") ? props.at("value") : 5.0;
            const bool currentMode = props.count("sourceType") ? props.at("sourceType") > 0.5 : false;
            drawCaptionBlock(painter,
                             QStringLiteral("Power Source"),
                             currentMode ? formatCurrent(value) : formatVoltage(value),
                             Qt::darkRed);
        }

        paintConnectionPoints(painter);
    }
};

class LightBulbItem : public GraphComponent {
public:
    explicit LightBulbItem(int id) : GraphComponent(id, "Bulb") {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());

        bool lit = false;
        double resistance = 100.0;
        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            resistance = props.count("resistance") ? props.at("resistance") : resistance;
            lit = props.count("isActive") ? props.at("isActive") > 0.5 : false;
        }

        painter->setBrush(lit ? QColor(255, 248, 160) : Qt::white);
        painter->drawEllipse(QRectF(-15, -15, 30, 30));
        painter->drawLine(QPointF(-10, -10), QPointF(10, 10));
        painter->drawLine(QPointF(-10, 10), QPointF(10, -10));

        if (auto* comp = model()) {
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatResistance(resistance), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

class AmmeterItem : public GraphComponent {
public:
    explicit AmmeterItem(int id) : GraphComponent(id, "Ammeter") {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());
        painter->setBrush(isSelected() ? QColor(220, 235, 255) : Qt::white);
        painter->drawEllipse(QRectF(-15, -15, 30, 30));
        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 12, QFont::Bold));
        painter->drawText(QRectF(-15, -15, 30, 30), Qt::AlignCenter, "A");

        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double current = props.count("current") ? props.at("current") : 0.0;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatCurrent(current), Qt::blue);
        }

        paintConnectionPoints(painter);
    }
};

class VoltmeterItem : public GraphComponent {
public:
    explicit VoltmeterItem(int id) : GraphComponent(id, "Voltmeter") {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());
        painter->setBrush(isSelected() ? QColor(220, 235, 255) : Qt::white);
        painter->drawEllipse(QRectF(-15, -15, 30, 30));
        painter->setPen(Qt::darkRed);
        painter->setFont(QFont("Arial", 12, QFont::Bold));
        painter->drawText(QRectF(-15, -15, 30, 30), Qt::AlignCenter, "V");

        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double voltage = props.count("voltage") ? props.at("voltage") : 0.0;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatVoltage(voltage), Qt::red);
        }

        paintConnectionPoints(painter);
    }
};

class CapacitorItem : public GraphComponent {
public:
    explicit CapacitorItem(int id) : GraphComponent(id, "Capacitor") {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        drawLead(painter, terminalLocalPos(0).x(), -5);
        drawLead(painter, 5, terminalLocalPos(1).x());
        painter->drawLine(QPointF(-5, -12), QPointF(-5, 12));
        painter->drawLine(QPointF(5, -12), QPointF(5, 12));

        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double c = props.count("capacitance") ? props.at("capacitance") : 1e-6;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatCapacitance(c), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

class InductorItem : public GraphComponent {
public:
    explicit InductorItem(int id) : GraphComponent(id, "Inductor") {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());
        painter->drawArc(QRectF(-15, -5, 10, 10), 0, 180 * 16);
        painter->drawArc(QRectF(-5, -5, 10, 10), 0, 180 * 16);
        painter->drawArc(QRectF(5, -5, 10, 10), 0, 180 * 16);

        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double l = props.count("inductance") ? props.at("inductance") : 1e-3;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatInductance(l), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

class SwitchItem : public GraphComponent {
public:
    explicit SwitchItem(int id) : GraphComponent(id, "Switch"), m_isClosed(false) {}

    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        drawLead(painter, terminalLocalPos(0).x(), -12);
        drawLead(painter, 12, terminalLocalPos(1).x());

        painter->setBrush(isSelected() ? QColor(220, 235, 255) : Qt::white);
        painter->drawEllipse(QRectF(-12, -3, 6, 6));
        painter->drawEllipse(QRectF(6, -3, 6, 6));
        if (m_isClosed) {
            painter->drawLine(QPointF(-9, 0), QPointF(9, 0));
        } else {
            painter->drawLine(QPointF(-9, 0), QPointF(8, -12));
        }

        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const bool closed = props.count("state") ? props.at("state") > 0.5 : m_isClosed;
            m_isClosed = closed;
            drawCaptionBlock(painter,
                             QString::fromStdString(comp->getName()),
                             closed ? QStringLiteral("CLOSED") : QStringLiteral("OPEN"),
                             closed ? Qt::darkRed : Qt::darkGray);
        } else {
            drawCaptionBlock(painter, QStringLiteral("Switch"), m_isClosed ? QStringLiteral("CLOSED") : QStringLiteral("OPEN"),
                             m_isClosed ? Qt::darkRed : Qt::darkGray);
        }

        paintConnectionPoints(painter);
    }

    void editProperties(QWidget* parent = nullptr) override {
        Q_UNUSED(parent);
        if (auto* comp = model()) {
            if (auto* sw = dynamic_cast<CircuitSim::Switch*>(comp)) {
                m_isClosed = !sw->isOn();
                sw->setState(m_isClosed);
                sw->setProperty("state", m_isClosed ? 1.0 : 0.0);
                if (GraphComponent::s_circuit) {
                    GraphComponent::s_circuit->solve();
                }
                if (auto* sc = scene()) {
                    sc->update();
                }
                update();
                updateConnectedWires();
            }
        }
    }

    bool m_isClosed;
};

#endif // COMPONENTITEMS_H
