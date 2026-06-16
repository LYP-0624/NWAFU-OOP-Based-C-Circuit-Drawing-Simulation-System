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
    Component* startComponent_;
    Component* endComponent_;
    Node* nodeA_;
    Node* nodeB_;
    int startPortIdx_;
    int endPortIdx_;
    double current_;
    double voltage_;
    double resistance_;

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
    double getVoltage() const;
    double getResistance() const { return resistance_; }
    void setResistance(double r) { resistance_ = r; }

    bool isConnected() const;
    
    std::string getCurrentDirection() const;
    
    double getPower() const;
    
    std::string getPowerFlow() const;
    
    bool isPowerAbsorbing() const;
    
    bool isPowerSupplying() const;
};

} // namespace CircuitSim

#endif