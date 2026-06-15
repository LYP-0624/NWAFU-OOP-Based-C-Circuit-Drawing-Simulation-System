#ifndef GRAPHCOMPONENT_H
#define GRAPHCOMPONENT_H

#include <QGraphicsItem>
#include <QPainter>
#include <QWidget>                 // 👈 强行补充：让编译器认识 QWidget
#include <QStyleOptionGraphicsItem> // 👈 强行补充：让编译器认识图形选项参数

// 核心契约：图形元器件多态基类
class GraphComponent : public QGraphicsItem
{
public:
    // 1. 构造函数：每个图形元件诞生时，必须强行绑定一个唯一的 ID 和 类型标签
    GraphComponent(int id, const QString& type);
    virtual ~GraphComponent() override = default;

    // 2. Qt 图形框架硬性要求的两大核心声明
    virtual QRectF boundingRect() const override;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override = 0;

    // 3. 后端数据契约解耦接口
    int getId() const { return m_id; }
    QString getType() const { return m_type; }

    // 实时接收同学A计算出的电流电压，用于刷新前端
    void updatePhysicsData(double voltage, double current);

protected:
    int m_id;           // 对应物理层的唯一 ID
    QString m_type;     // 元件类型："Resistor", "Bulb", "Power" 等
    double m_voltage;   // 缓存当前电压
    double m_current;   // 缓存当前电流
};

#endif // GRAPHCOMPONENT_H