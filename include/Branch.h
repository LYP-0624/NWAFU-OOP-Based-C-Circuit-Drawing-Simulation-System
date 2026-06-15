#ifndef BRANCH_H
#define BRANCH_H

#include <memory>
#include <string>

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
    double voltage_;

public:
    Branch(int id, Component* component, Node* nodeA, Node* nodeB);
    ~Branch() = default;

    int getId() const { return id_; }
    Component* getComponent() const { return component_; }
    Node* getNodeA() const { return nodeA_; }
    Node* getNodeB() const { return nodeB_; }
    double getCurrent() const { return current_; }
    void setCurrent(double current) { current_ = current; }
    double getVoltage() const;

    bool isConnected() const;
    
    std::string getCurrentDirection() const;
    
    double getPower() const;
    
    std::string getPowerFlow() const;
    
    bool isPowerAbsorbing() const;
    
    bool isPowerSupplying() const;
};

} // namespace CircuitSim

#endif