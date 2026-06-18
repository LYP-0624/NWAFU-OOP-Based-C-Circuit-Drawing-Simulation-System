#ifndef CIRCUIT_H
#define CIRCUIT_H

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Branch.h"
#include "SimulationResult.h"

namespace CircuitSim {

class Component;
class Node;
class Port;
class MNASolver;

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
    std::string lastError_;
    MNASolver* solver_;

    void buildConnectivityGraph();

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

    void addNode(Node* node) { if (node) nodes_.push_back(node); }
    const std::vector<Node*>& getNodes() const { return nodes_; }

    void addBranch(Branch* branch) { if (branch) branches_.push_back(branch); }
    const std::vector<Branch*>& getBranches() const { return branches_; }
    Branch* getBranch(int id) const;
    bool removeBranch(int id);

    Branch* connect(Port* a, Port* b);
    void disconnect(Port* port);

    bool solve();
    bool isSolved() const { return solved_; }
    std::string getLastError() const { return lastError_; }
    std::vector<SimulationResult> getResults() const;
    const std::vector<SimulationResult>& getSimulationResults() const { return simulation_results_; }

    SimulationResult queryById(int id) const;
    std::vector<SimulationResult> queryByType(const std::string& type) const;

    bool isValid() const;
    int getNodeCount() const { return static_cast<int>(nodes_.size()); }
    int getComponentCount() const { return static_cast<int>(components_.size()); }
    int getBranchCount() const { return static_cast<int>(branches_.size()); }

    int getNextComponentId() const { return nextComponentId_; }
    int getNextNodeId() const { return nextNodeId_; }
    int getNextBranchId() const { return nextBranchId_; }
    void setIdCounters(int nextComponentId, int nextNodeId, int nextBranchId);

    void clear();

    std::string toJson() const;
    bool fromJson(const std::string& json);
    bool saveToJsonFile(const std::string& file_path);
    bool loadFromJsonFile(const std::string& file_path);

    double getNodeVoltage(int nodeId) const;

    void logSimulation(const std::string& description = "") const;

    std::vector<std::string> validateCircuit() const;
};

} // namespace CircuitSim

#endif
