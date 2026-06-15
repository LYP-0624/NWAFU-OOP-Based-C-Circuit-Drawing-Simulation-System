#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include "SimulationResult.h"

namespace CircuitSim {

class Component;
class Node;
class Port;
class MNASolver;

class Circuit {
private:
    std::unordered_map<int, Component*> components_;
    std::vector<Node*> nodes_;
    int nextComponentId_;
    int nextNodeId_;
    bool solved_;
    MNASolver* solver_;

    void buildConnectivityGraph();
    void assignGroundNode();

public:
    Circuit();
    ~Circuit();

    void addComponent(Component* comp);
    void removeComponent(int id);
    Component* getComponent(int id) const;
    const std::unordered_map<int, Component*>& getAllComponents() const { return components_; }

    Node* createNode();
    void connect(Port* a, Port* b);
    void disconnect(Port* port);
    void mergeNodes(Node* a, Node* b);

    bool solve();
    bool isSolved() const { return solved_; }
    std::vector<SimulationResult> getResults() const;

    SimulationResult queryById(int id) const;
    std::vector<SimulationResult> queryByType(const std::string& type) const;

    bool isValid() const;
    bool hasGround() const;
    int getNodeCount() const { return static_cast<int>(nodes_.size()); }

    void clear();

    std::string toJson() const;
    bool fromJson(const std::string& json);
};

} // namespace CircuitSim

#endif