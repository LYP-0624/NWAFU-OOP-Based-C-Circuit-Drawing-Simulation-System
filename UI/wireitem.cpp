#include "wireitem.h"

#include <QPainter>
#include <QPainterPath>
#include <QStyleOptionGraphicsItem>

#include "graphcomponent.h"

// 构造函数：创建一条连接两个元件端口的完整导线（非临时状态）
// startItem: 起始端元件图形对象指针
// startPort: 起始端在元件上的端口号（0或1，对应元件的两个引脚）
// endItem:   终止端元件图形对象指针
// endPort:   终止端在元件上的端口号
// branchId:  该导线在电路拓扑中的支路编号，用于仿真计算
WireItem::WireItem(GraphComponent* startItem, int startPort, GraphComponent* endItem, int endPort, int branchId)
    : m_startItem(startItem),
      m_endItem(endItem),
      m_startPort(startPort),
      m_endPort(endPort),
      m_branchId(branchId),
      m_temporary(false) {
    // 设置图元叠放次序为-1，确保导线绘制在元件图元下方，避免遮挡元件
    setZValue(-1.0);
    // 设置导线画笔：深灰色、线宽2.0像素
    setPen(QPen(QColor(40, 40, 40), 2.0));
    // 启用图元可选中标志，允许用户点击选中该导线
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    // 设置只接受左键鼠标事件
    setAcceptedMouseButtons(Qt::LeftButton);
    // 若起始元件存在，将该导线注册到起始元件的连线列表中，便于元件移动时同步更新导线
    if (m_startItem) m_startItem->addWire(this);
    // 若终止元件存在，同样注册到终止元件的连线列表中
    if (m_endItem) m_endItem->addWire(this);
    // 立即计算并更新导线的几何路径（贝塞尔曲线）
    updateGeometry();
}

// 构造函数：创建一条临时导线（拖拽绘制过程中使用，仅连接起始元件，终点跟随鼠标）
// startItem: 起始端元件图形对象指针
// startPort: 起始端在元件上的端口号
// endPoint:  鼠标当前位置（场景坐标），作为临时终点
WireItem::WireItem(GraphComponent* startItem, int startPort, const QPointF& endPoint)
    : m_startItem(startItem),
      m_endItem(nullptr),
      m_endPoint(endPoint),
      m_startPort(startPort),
      m_endPort(1),
      m_branchId(-1),
      m_temporary(true) {
    // 设置图元叠放次序为-1，确保导线在元件下方
    setZValue(-1.0);
    // 设置临时导线画笔：浅灰色、线宽1.5像素、虚线样式，视觉上与实线导线区分
    setPen(QPen(QColor(90, 90, 90), 1.5, Qt::DashLine));
    // 临时导线不可被选中，避免拖拽过程中误选
    setFlag(QGraphicsItem::ItemIsSelectable, false);
    // 临时导线不响应任何鼠标按钮事件
    setAcceptedMouseButtons(Qt::NoButton);
    // 若起始元件存在，将临时导线注册到起始元件的连线列表
    if (m_startItem) m_startItem->addWire(this);
    // 根据当前鼠标位置计算并绘制临时路径
    updateGeometry();
}

// 析构函数：在导线对象销毁前，从两端元件的连线列表中移除自身引用，防止悬空指针
WireItem::~WireItem() {
    if (m_startItem) {
        m_startItem->removeWire(this);
    }
    if (m_endItem) {
        m_endItem->removeWire(this);
    }
}

// 设置临时终点坐标（用于拖拽过程中实时更新导线终点位置）
// endPoint: 鼠标当前的场景坐标位置
void WireItem::setEndPoint(const QPointF& endPoint) {
    m_endPoint = endPoint;
    // 标记为临时状态，确保绘制时使用虚线样式
    m_temporary = true;
    // 重新计算导线路径以匹配新的终点位置
    updateGeometry();
}

// 重写绘制函数：根据选中状态和临时状态动态调整导线外观
// painter: Qt 绘图引擎指针
// option:  样式选项，包含选中状态等信息
// widget:  父控件指针（通常为空）
void WireItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) {
    // 获取当前已设置的画笔
    QPen pen = this->pen();
    // 若导线处于选中状态，改用橙色粗线高亮显示
    if (isSelected()) {
        pen = QPen(QColor(220, 80, 40), 3.0);
    } else if (m_temporary) {
        // 若导线为临时状态（拖拽中），使用浅灰色虚线
        pen = QPen(QColor(90, 90, 90), 1.5, Qt::DashLine);
    }
    // 应用更新后的画笔设置
    setPen(pen);
    // 调用基类 QGraphicsPathItem 的默认绘制逻辑，沿已设置的路径绘制导线
    QGraphicsPathItem::paint(painter, option, widget);
}

// 更新导线几何路径：根据两端元件端口的当前位置计算贝塞尔曲线路径
void WireItem::updateGeometry() {
    // 通知 Qt 图形框架准备更新几何区域，触发重绘前的边界框重新计算
    prepareGeometryChange();

    // 计算起点坐标：从起始元件的指定端口获取场景坐标；若起始元件不存在则使用原点(0,0)
    const QPointF p1 = m_startItem ? m_startItem->terminalScenePos(m_startPort) : QPointF();
    // 计算终点坐标：临时导线使用鼠标位置(m_endPoint)，否则从终止元件端口获取场景坐标
    const QPointF p2 = m_temporary || !m_endItem ? m_endPoint : m_endItem->terminalScenePos(m_endPort);

    // 计算贝塞尔曲线的控制点：先取起点和终点的中点
    QPointF control = (p1 + p2) * 0.5;
    // 计算从起点到终点的方向向量
    const QPointF delta = p2 - p1;
    // 将方向向量逆时针旋转90度（交换x,y并取反y），再缩放15%，使控制点偏离直线中点
    // 这样生成的二次贝塞尔曲线会呈现自然的弧线效果，而非僵硬的直线
    control += QPointF(-delta.y(), delta.x()) * 0.15;

    // 创建绘图路径对象
    QPainterPath path;
    // 将画笔移动到起点
    path.moveTo(p1);
    // 使用二次贝塞尔曲线（quadTo）从起点绘制到终点，control为控制点
    path.quadTo(control, p2);
    // 将计算好的路径设置到本图元，Qt 会自动根据此路径绘制导线
    setPath(path);
}