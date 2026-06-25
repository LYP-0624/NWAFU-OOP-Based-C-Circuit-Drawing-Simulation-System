#ifndef PORT_H
#define PORT_H

namespace CircuitSim {

class Component;
class Node;

// 元件端口
class Port {
private:
    Component* parent_;  // 所属元件
    int index_;          // 端口索引
    Node* node_;         // 连接的节点
    bool connected_;     // 是否已连接

public:
    Port(Component* parent, int index);
    ~Port();

    Component* getParent() const { return parent_; }
    int getIndex() const { return index_; }
    Node* getNode() const { return node_; }
    bool isConnected() const { return connected_; }

    void connectTo(Node* node);  // 连接到节点
    void disconnect();           // 断开连接
    double getVoltage() const;   // 获取端口电压
};

} // namespace CircuitSim

#endif
