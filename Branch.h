#ifndef BRANCH_H
#define BRANCH_H

#include <memory>

namespace CircuitSim {

class Component;
class Node;

class Branch {
private:
    int id_;
    Component* component_;
    Node* nodeA_;
    Node* nodeB_;
    double current_;

public:
    Branch(int id, Component* component, Node* nodeA, Node* nodeB);
    ~Branch() = default;

    int getId() const { return id_; }
    Component* getComponent() const { return component_; }
    Node* getNodeA() const { return nodeA_; }
    Node* getNodeB() const { return nodeB_; }
    double getCurrent() const { return current_; }
    void setCurrent(double current) { current_ = current; }

    bool isConnected() const;
    double getVoltage() const;
};

} // namespace CircuitSim

#endif