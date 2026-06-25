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

// 电路管理器
class Circuit {
private:
    std::string name_;                           // 电路名称
    std::unordered_map<int, Component*> components_; // 元件集合
    std::vector<Node*> nodes_;                   // 节点列表
    std::vector<Branch*> branches_;              // 支路列表
    std::vector<SimulationResult> simulation_results_; // 仿真结果
    int nextComponentId_;                        // 下一个元件ID
    int nextNodeId_;                             // 下一个节点ID
    int nextBranchId_;                           // 下一个支路ID
    bool solved_;                                // 是否已求解
    std::string lastError_;                      // 最后错误信息
    MNASolver* solver_;                          // MNA求解器

    void buildConnectivityGraph();               // 构建连接图

public:
    Circuit();
    ~Circuit();

    std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }

    void addComponent(Component* comp);          // 添加元件
    void removeComponent(int id);                // 移除元件
    Component* getComponent(int id) const;       // 获取元件
    const std::unordered_map<int, Component*>& getAllComponents() const { return components_; }
    const std::unordered_map<int, Component*>& getComponents() const { return components_; }

    void addNode(Node* node) { if (node) nodes_.push_back(node); }
    const std::vector<Node*>& getNodes() const { return nodes_; }

    void addBranch(Branch* branch) { if (branch) branches_.push_back(branch); }
    const std::vector<Branch*>& getBranches() const { return branches_; }
    Branch* getBranch(int id) const;             // 获取支路
    bool removeBranch(int id);                   // 移除支路

    Branch* connect(Port* a, Port* b);           // 连接两个端口
    void disconnect(Port* port);                 // 断开端口连接

    bool solve();                                // 执行电路仿真
    bool isSolved() const { return solved_; }
    std::string getLastError() const { return lastError_; }
    std::vector<SimulationResult> getResults() const;
    const std::vector<SimulationResult>& getSimulationResults() const { return simulation_results_; }

    SimulationResult queryById(int id) const;    // 按ID查询结果
    std::vector<SimulationResult> queryByType(const std::string& type) const; // 按类型查询

    bool isValid() const;
    int getNodeCount() const { return static_cast<int>(nodes_.size()); }
    int getComponentCount() const { return static_cast<int>(components_.size()); }
    int getBranchCount() const { return static_cast<int>(branches_.size()); }

    int getNextComponentId() const { return nextComponentId_; }
    int getNextNodeId() const { return nextNodeId_; }
    int getNextBranchId() const { return nextBranchId_; }
    void setIdCounters(int nextComponentId, int nextNodeId, int nextBranchId);

    void clear();                                // 清空电路

    std::string toJson() const;                  // 转换为JSON
    bool fromJson(const std::string& json);      // 从JSON恢复
    bool saveToJsonFile(const std::string& file_path); // 保存到JSON文件
    bool loadFromJsonFile(const std::string& file_path); // 从JSON文件加载

    double getNodeVoltage(int nodeId) const;     // 获取节点电压

    void logSimulation(const std::string& description = "") const; // 记录仿真

    std::vector<std::string> validateCircuit() const; // 验证电路
};

} // namespace CircuitSim

#endif
