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
    std::string name_;
    std::unordered_map<int, Component*> components_;
    std::vector<Node*> nodes_;
    std::vector<Branch*> branches_;
    std::vector<SimulationResult> simulation_results_;
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

    std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    void addComponent(Component* comp);
    void removeComponent(int id);
    Component* getComponent(int id) const;
    const std::unordered_map<int, Component*>& getAllComponents() const { return components_; }
    const std::unordered_map<int, Component*>& getComponents() const { return components_; }

    Node* createNode();
    void addNode(Node* node) { nodes_.push_back(node); }
    const std::vector<Node*>& getNodes() const { return nodes_; }
    
    void addBranch(Branch* branch) { branches_.push_back(branch); }
    const std::vector<Branch*>& getBranches() const { return branches_; }
    
    void connect(Port* a, Port* b);
    void disconnect(Port* port);
    void mergeNodes(Node* a, Node* b);

    bool solve();
    bool isSolved() const { return solved_; }
    std::vector<SimulationResult> getResults() const;
    const std::vector<SimulationResult>& getSimulationResults() const { return simulation_results_; }

    SimulationResult queryById(int id) const;
    std::vector<SimulationResult> queryByType(const std::string& type) const;

    bool isValid() const;
    bool hasGround() const;
    int getNodeCount() const { return static_cast<int>(nodes_.size()); }
    int getComponentCount() const { return static_cast<int>(components_.size()); }

    void clear();

    std::string toJson() const;
    bool fromJson(const std::string& json);
    bool saveToJsonFile(const std::string& file_path);
    bool loadFromJsonFile(const std::string& file_path);

    const std::vector<Branch*>& getBranchList() const;
    
    double getNodeVoltage(int nodeId) const;
    
    void logSimulation(const std::string& description = "") const;
    
    std::vector<std::string> validateCircuit() const;
};

} // namespace CircuitSim

#endif