#include "graphcomponent.h"

// 构造函数实现：初始化基本属性，并一键开启 Qt 最迷人的“鼠标拖拽”魔力！
GraphComponent::GraphComponent(int id, const QString& type)
    : m_id(id), m_type(type), m_voltage(0.0), m_current(0.0)
{
    // 一行代码直接激活 Qt 底层的自由拖拽与选中属性 [cite: 493]！
    setFlags(QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsSelectable);
}

// 规定所有电路元器件符号的“通用点击物理边界框”（标准 80x50 像素的黄金比例元件舞台）
QRectF GraphComponent::boundingRect() const
{
    return QRectF(-40, -25, 80, 50);
}

// 接收后台物理引擎的数据刷新接口
void GraphComponent::updatePhysicsData(double voltage, double current)
{
    m_voltage = voltage;
    m_current = current;
    update(); // 强制触发 Qt 重新调用 paint() 重绘皮肤，实现动态视觉反馈 [cite: 412, 456]！
}