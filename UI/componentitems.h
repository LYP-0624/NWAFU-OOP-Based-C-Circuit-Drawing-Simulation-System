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

// 将电阻值格式化为带单位的字符串（自动切换MΩ/kΩ/Ω）
inline QString formatResistance(double ohms) {
    if (ohms >= 1000000.0) return QString("%1 MOhm").arg(ohms / 1000000.0, 0, 'f', 1);
    if (ohms >= 1000.0) return QString("%1 kOhm").arg(ohms / 1000.0, 0, 'f', 1);
    return QString("%1 Ohm").arg(ohms, 0, 'f', 0);
}

// 将电压值格式化为带V单位的字符串
inline QString formatVoltage(double volts) {
    return QString("%1 V").arg(volts, 0, 'f', 1);
}

// 将电流值格式化为带A单位的字符串（保留3位小数）
inline QString formatCurrent(double amps) {
    return QString("%1 A").arg(amps, 0, 'f', 3);
}

// 将电容值格式化为带单位的字符串（自动切换mF/μF/F）
inline QString formatCapacitance(double farads) {
    if (farads >= 1e-3) return QString("%1 mF").arg(farads * 1000.0, 0, 'f', 3);
    if (farads >= 1e-6) return QString("%1 uF").arg(farads * 1e6, 0, 'f', 3);
    return QString("%1 F").arg(farads, 0, 'g', 3);
}

// 将电感值格式化为带单位的字符串（自动切换H/mH/μH）
inline QString formatInductance(double henry) {
    if (henry >= 1.0) return QString("%1 H").arg(henry, 0, 'f', 3);
    if (henry >= 1e-3) return QString("%1 mH").arg(henry * 1000.0, 0, 'f', 3);
    return QString("%1 uH").arg(henry * 1e6, 0, 'f', 3);
}

// 在元器件下方绘制标题和数值文本块
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

// 绘制元器件引脚线段（水平方向）
inline void drawLead(QPainter* painter, qreal fromX, qreal toX) {
    painter->drawLine(QPointF(fromX, 0), QPointF(toX, 0));
}

} // namespace

// 电阻图形项类
class ResistorItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID
    explicit ResistorItem(int id) : GraphComponent(id, "Resistor") {}

    // 绘制电阻图形：锯齿形电阻符号+引脚+参数标注
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -24);
        drawLead(painter, 24, terminalLocalPos(1).x());

        // 绘制锯齿形电阻本体
        const QPointF pts[] = {
            QPointF(-24, 0), QPointF(-18, -8), QPointF(-12, 8), QPointF(-6, -8),
            QPointF(0, 8), QPointF(6, -8), QPointF(12, 8), QPointF(18, -8), QPointF(24, 0)
        };
        const int pointCount = static_cast<int>(sizeof(pts) / sizeof(pts[0]));
        for (int i = 0; i + 1 < pointCount; ++i) {
            painter->drawLine(pts[i], pts[i + 1]);
        }

        // 绘制名称和电阻值标注
        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double r = props.count("resistance") ? props.at("resistance") : 100.0;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatResistance(r), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

// 电源图形项类
class PowerSourceItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID
    explicit PowerSourceItem(int id) : GraphComponent(id, "PowerSource") {}

    // 绘制电源图形：长线短线符号+引脚+参数标注
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -18);
        drawLead(painter, 18, terminalLocalPos(1).x());

        // 绘制电源符号（一长一短竖线）
        painter->drawLine(QPointF(-10, -14), QPointF(-10, 14));
        painter->drawLine(QPointF(6, -8), QPointF(6, 8));

        // 绘制电压/电流值标注
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

// 灯泡图形项类
class LightBulbItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID
    explicit LightBulbItem(int id) : GraphComponent(id, "Bulb") {}

    // 绘制灯泡图形：圆形+交叉线，根据isActive改变填充色
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());

        // 根据仿真结果判断是否点亮（改变填充色）
        bool lit = false;
        double resistance = 100.0;
        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            resistance = props.count("resistance") ? props.at("resistance") : resistance;
            lit = props.count("isActive") ? props.at("isActive") > 0.5 : false;
        }

        // 绘制圆形灯泡本体，点亮时为黄色，熄灭时为白色
        painter->setBrush(lit ? QColor(255, 248, 160) : Qt::white);
        painter->drawEllipse(QRectF(-15, -15, 30, 30));
        // 绘制灯泡内部交叉线
        painter->drawLine(QPointF(-10, -10), QPointF(10, 10));
        painter->drawLine(QPointF(-10, 10), QPointF(10, -10));

        // 绘制名称和电阻值标注
        if (auto* comp = model()) {
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatResistance(resistance), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

// 电流表图形项类
class AmmeterItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID
    explicit AmmeterItem(int id) : GraphComponent(id, "Ammeter") {}

    // 绘制电流表图形：圆圈内字母A+电流值标注
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());
        // 绘制圆形表盘，选中时高亮
        painter->setBrush(isSelected() ? QColor(220, 235, 255) : Qt::white);
        painter->drawEllipse(QRectF(-15, -15, 30, 30));
        // 绘制字母A
        painter->setPen(Qt::darkBlue);
        painter->setFont(QFont("Arial", 12, QFont::Bold));
        painter->drawText(QRectF(-15, -15, 30, 30), Qt::AlignCenter, "A");

        // 绘制名称和电流读数
        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double current = props.count("current") ? props.at("current") : 0.0;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatCurrent(current), Qt::blue);
        }

        paintConnectionPoints(painter);
    }
};

// 电压表图形项类
class VoltmeterItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID
    explicit VoltmeterItem(int id) : GraphComponent(id, "Voltmeter") {}

    // 绘制电压表图形：圆圈内字母V+电压值标注
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());
        // 绘制圆形表盘，选中时高亮
        painter->setBrush(isSelected() ? QColor(220, 235, 255) : Qt::white);
        painter->drawEllipse(QRectF(-15, -15, 30, 30));
        // 绘制字母V
        painter->setPen(Qt::darkRed);
        painter->setFont(QFont("Arial", 12, QFont::Bold));
        painter->drawText(QRectF(-15, -15, 30, 30), Qt::AlignCenter, "V");

        // 绘制名称和电压读数
        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double voltage = props.count("voltage") ? props.at("voltage") : 0.0;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatVoltage(voltage), Qt::red);
        }

        paintConnectionPoints(painter);
    }
};

// 电容图形项类
class CapacitorItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID
    explicit CapacitorItem(int id) : GraphComponent(id, "Capacitor") {}

    // 绘制电容图形：两条平行竖线+引脚+容值标注
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -5);
        drawLead(painter, 5, terminalLocalPos(1).x());
        // 绘制两条平行极板
        painter->drawLine(QPointF(-5, -12), QPointF(-5, 12));
        painter->drawLine(QPointF(5, -12), QPointF(5, 12));

        // 绘制名称和电容值标注
        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double c = props.count("capacitance") ? props.at("capacitance") : 1e-6;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatCapacitance(c), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

// 电感图形项类
class InductorItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID
    explicit InductorItem(int id) : GraphComponent(id, "Inductor") {}

    // 绘制电感图形：三个半圆弧+引脚+感值标注
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));
        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -15);
        drawLead(painter, 15, terminalLocalPos(1).x());
        // 绘制三个半圆弧组成电感符号
        painter->drawArc(QRectF(-15, -5, 10, 10), 0, 180 * 16);
        painter->drawArc(QRectF(-5, -5, 10, 10), 0, 180 * 16);
        painter->drawArc(QRectF(5, -5, 10, 10), 0, 180 * 16);

        // 绘制名称和电感值标注
        if (auto* comp = model()) {
            const auto props = comp->getAllProperties();
            const double l = props.count("inductance") ? props.at("inductance") : 1e-3;
            drawCaptionBlock(painter, QString::fromStdString(comp->getName()), formatInductance(l), Qt::darkGreen);
        }

        paintConnectionPoints(painter);
    }
};

// 开关图形项类
class SwitchItem : public GraphComponent {
public:
    // 构造函数：传入元器件ID，初始状态为断开
    explicit SwitchItem(int id) : GraphComponent(id, "Switch"), m_isClosed(false) {}

    // 绘制开关图形：两个触点+斜线/直线表示通断状态+状态标注
    void paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) override {
        painter->setRenderHint(QPainter::Antialiasing, true);
        painter->setPen(QPen(Qt::black, 2));

        // 绘制左右引脚线
        drawLead(painter, terminalLocalPos(0).x(), -12);
        drawLead(painter, 12, terminalLocalPos(1).x());

        // 绘制两个圆形触点
        painter->setBrush(isSelected() ? QColor(220, 235, 255) : Qt::white);
        painter->drawEllipse(QRectF(-12, -3, 6, 6));
        painter->drawEllipse(QRectF(6, -3, 6, 6));
        // 根据通断状态绘制连接线（闭合为水平线，断开为斜线）
        if (m_isClosed) {
            painter->drawLine(QPointF(-9, 0), QPointF(9, 0));
        } else {
            painter->drawLine(QPointF(-9, 0), QPointF(8, -12));
        }

        // 绘制名称和开关状态（CLOSED/OPEN）
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

    // 双击或编辑时切换开关状态，触发重新仿真
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

    bool m_isClosed;  // 开关当前闭合状态
};

#endif // COMPONENTITEMS_H