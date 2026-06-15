#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <vector>
#include <map>
#include <unordered_map>
#include <string>
#include <memory>
#include "SimulationResult.h"
#include "Branch.h"

namespace CircuitSim {

class Component;
class Node;
class Port;
class MNASolver;
class CircuitValidator;

class Circuit {
private:
    std::unordered_map<int, Component*> components_;
    std::vector<Node*> nodes_;
    std::vector<Branch*> branches_;
    int nextComponentId_;
    int nextNodeId_;
    int nextBranchId_;
    bool solved_;
    MNASolver* solver_;

    void buildConnectivityGraph();
    void assignGroundNode();
    void rebuildBranches();

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
    int getComponentCount() const { return static_cast<int>(components_.size()); }

    void clear();

    std::string toJson() const;
    bool fromJson(const std::string& json);

    const std::vector<Branch*>& getBranchList() const;
    
    double getNodeVoltage(int nodeId) const;
    
    void logSimulation(const std::string& description = "") const;
    
    std::vector<std::string> validateCircuit() const;
};

} // namespace CircuitSim

#endif