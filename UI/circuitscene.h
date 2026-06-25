#ifndef CIRCUITSCENE_H
#define CIRCUITSCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QPointF>

#include "Circuit.h"

class QGraphicsSceneMouseEvent;
class GraphComponent;
class WireItem;

// 电路画布场景
class CircuitScene : public QGraphicsScene {
public:
    // 工具类型枚举
    enum class Tool {
        Select,       // 选择工具
        Resistor,     // 电阻放置
        PowerSource,  // 电源放置
        Bulb,         // 灯泡放置
        Ammeter,      // 电流表放置
        Voltmeter,    // 电压表放置
        Capacitor,    // 电容放置
        Inductor,     // 电感放置
        Switch        // 开关放置
    };

    explicit CircuitScene(QObject* parent = nullptr);

    void setCircuit(CircuitSim::Circuit* circuit);
    CircuitSim::Circuit* circuit() const { return m_circuit; }

    void setTool(Tool tool) { m_tool = tool; }
    Tool tool() const { return m_tool; }

    void syncFromCircuit();           // 从电路数据同步到视图
    void refreshVisuals();            // 刷新视觉显示
    void deleteSelectedItems();       // 删除选中项
    void cancelPlacementMode();       // 取消放置模式

    GraphComponent* componentItem(int componentId) const; // 获取图形元件

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
    CircuitSim::Circuit* m_circuit;       // 电路模型
    Tool m_tool;                          // 当前工具
    GraphComponent* m_wireStartItem;      // 导线起点元件
    int m_wireStartPort;                  // 导线起点端口
    WireItem* m_previewWire;              // 预览导线
    QMap<int, GraphComponent*> m_componentItems; // 图形元件映射
    QMap<int, WireItem*> m_wireItems;     // 导线映射

    GraphComponent* createComponentItem(CircuitSim::Component* component); // 创建图形元件
    void createWireFromBranch(const CircuitSim::Branch* branch);           // 从支路创建导线
    void removeWireItem(WireItem* wire);                                   // 移除导线
    void removeComponentItem(GraphComponent* component);                   // 移除元件
    void beginWireDrag(GraphComponent* component, int terminal, const QPointF& scenePos); // 开始拖拽导线
    void finishWireDrag(const QPointF& scenePos);                          // 完成导线连接
    void cancelWireDrag();                                                 // 取消导线拖拽
    void placeComponentAt(const QPointF& scenePos);                        // 在指定位置放置元件
    GraphComponent* componentAt(const QPointF& scenePos, int* terminal = nullptr) const; // 获取指定位置的元件
};

#endif // CIRCUITSCENE_H
