#ifndef BRANCH_H
#define BRANCH_H

#include <string>

namespace CircuitSim {

class Component;
class Node;

// 电路支路
class Branch {
private:
    int id_;                  // 支路ID
    Component* component_;    // 关联的元件(旧式构造)
    Component* startComponent_; // 起点元件
    Component* endComponent_;   // 终点元件
    Node* nodeA_;             // 节点A
    Node* nodeB_;             // 节点B
    int startPortIdx_;        // 起点端口索引
    int endPortIdx_;          // 终点端口索引
    double current_;          // 电流(A)
    double voltage_;          // 电压(V)
    double resistance_;       // 电阻(Ω)

public:
    Branch(int id, Component* component, Node* nodeA, Node* nodeB);
    Branch(int id, Component* startComp, int startPortIdx, Component* endComp, int endPortIdx);
    ~Branch() = default;

    int getId() const { return id_; }
    Component* getComponent() const { return component_; }
    Component* getStartComponent() const { return startComponent_ ? startComponent_ : component_; }
    Component* getEndComponent() const { return endComponent_ ? endComponent_ : component_; }
    Node* getNodeA() const { return nodeA_; }
    Node* getNodeB() const { return nodeB_; }
    int getStartPortIndex() const { return startPortIdx_; }
    int getEndPortIndex() const { return endPortIdx_; }
    double getCurrent() const { return current_; }
    void setCurrent(double current) { current_ = current; }
    double getVoltage() const;   // 计算支路电压
    double getResistance() const { return resistance_; }
    void setResistance(double r) { resistance_ = r; }

    void setNodeA(Node* node) { nodeA_ = node; }
    void setNodeB(Node* node) { nodeB_ = node; }

    bool isConnected() const;           // 判断是否已连接

    std::string getCurrentDirection() const; // 获取电流方向

    double getPower() const;            // 计算功率
    std::string getPowerFlow() const;   // 获取功率流向
    bool isPowerAbsorbing() const;      // 是否吸收功率
    bool isPowerSupplying() const;      // 是否提供功率
};

} // namespace CircuitSim

#endif
