#ifndef PORT_H
#define PORT_H

namespace CircuitSim {

class Component;
class Node;

class Port {
private:
    Component* parent_;
    int index_;
    Node* node_;
    bool connected_;

public:
    Port(Component* parent, int index);
    ~Port();

    Component* getParent() const { return parent_; }
    int getIndex() const { return index_; }
    Node* getNode() const { return node_; }
    bool isConnected() const { return connected_; }

    void connectTo(Node* node);
    void disconnect();
    double getVoltage() const;
};

} // namespace CircuitSim

#endif