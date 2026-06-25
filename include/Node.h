#ifndef NODE_H
#define NODE_H

#include <vector>

namespace CircuitSim {

class Port;

// 电路节点
class Node {
private:
    int id_;              // 节点ID
    std::vector<Port*> ports_; // 连接到该节点的端口
    double voltage_;      // 节点电压(V)
    bool isGround_;       // 是否为地节点

public:
    explicit Node(int id);
    ~Node();

    int getId() const { return id_; }
    double getVoltage() const { return voltage_; }
    void setVoltage(double v) { voltage_ = v; }
    bool isGround() const { return isGround_; }
    void setGround(bool g) { isGround_ = g; }

    void addPort(Port* port);      // 添加端口
    void removePort(Port* port);   // 移除端口
    const std::vector<Port*>& getPorts() const { return ports_; }
    size_t getComponentCount() const; // 获取连接的元件数
};

} // namespace CircuitSim

#endif
