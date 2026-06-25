#include "Circuit.h"

#include <map>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <utility>

#include "CircuitValidator.h"
#include "Component.h"
#include "ComponentFactory.h"
#include "MNASolver.h"
#include "Node.h"
#include "Port.h"
#include "SimulationLogger.h"
#include "../Serialize/CircuitJsonSerializer.h"

namespace CircuitSim {

namespace {

// 并查集数据结构，用于构建连通分量
struct UnionFind {
    std::vector<int> parent;
    std::vector<int> rank;

    // 初始化并查集，每个元素初始父节点为自己
    explicit UnionFind(int size) : parent(size), rank(size, 0) {
        for (int i = 0; i < size; ++i) {
            parent[i] = i;
        }
    }

    // 查找根节点（带路径压缩）
    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    // 合并两个集合（按秩合并）
    void unite(int a, int b) {
        a = find(a);
        b = find(b);
        if (a == b) {
            return;
        }
        if (rank[a] < rank[b]) {
            std::swap(a, b);
        }
        parent[b] = a;
        if (rank[a] == rank[b]) {
            ++rank[a];
        }
    }
};

// 端口标识键（用于唯一标识一个元器件的某个端口）
struct PortKey {
    int componentId = -1;
    int portIndex = -1;

    // 重载小于运算符，用于map排序
    bool operator<(const PortKey& other) const {
        if (componentId != other.componentId) {
            return componentId < other.componentId;
        }
        return portIndex < other.portIndex;
    }
};

} // namespace

// 构造函数：初始化ID计数器和求解器
Circuit::Circuit()
    : nextComponentId_(1), nextNodeId_(1), nextBranchId_(1), solved_(false), solver_(new MNASolver()) {}

// 析构函数：清理所有资源
Circuit::~Circuit() {
    clear();
    delete solver_;
}
 
// 添加元器件到电路
void Circuit::addComponent(Component* comp) {
    if (!comp) {
        return;
    }
    components_[comp->getId()] = comp;
    nextComponentId_ = std::max(nextComponentId_, comp->getId() + 1);  // 更新ID计数器
    solved_ = false;  // 标记需要重新求解
}

// 移除指定ID的元器件，同时删除关联的支路
void Circuit::removeComponent(int id) {
    auto it = components_.find(id);
    if (it == components_.end()) {
        return;
    }

    // 删除与该元器件关联的所有支路
    branches_.erase(std::remove_if(branches_.begin(), branches_.end(), [id](Branch* branch) {
        if (!branch) {
            return true;
        }
        const bool match = branch->getStartComponent() && branch->getStartComponent()->getId() == id ||
                           branch->getEndComponent() && branch->getEndComponent()->getId() == id ||
                           branch->getComponent() && branch->getComponent()->getId() == id;
        if (match) {
            delete branch;
        }
        return match;
    }), branches_.end());

    delete it->second;
    components_.erase(it);
    buildConnectivityGraph();  // 重建连接图
    solved_ = false;
}

// 获取指定ID的元器件
Component* Circuit::getComponent(int id) const {
    auto it = components_.find(id);
    return it == components_.end() ? nullptr : it->second;
}

// 获取指定ID的支路
Branch* Circuit::getBranch(int id) const {
    for (auto* branch : branches_) {
        if (branch && branch->getId() == id) {
            return branch;
        }
    }
    return nullptr;
}

// 移除指定ID的支路
bool Circuit::removeBranch(int id) {
    for (auto it = branches_.begin(); it != branches_.end(); ++it) {
        if (*it && (*it)->getId() == id) {
            delete *it;
            branches_.erase(it);
            buildConnectivityGraph();
            solved_ = false;
            return true;
        }
    }
    return false;
}

// 连接两个端口，创建新的支路
Branch* Circuit::connect(Port* a, Port* b) {
    if (!a || !b || a == b) {
        return nullptr;
    }

    auto* branch = new Branch(nextBranchId_++, a->getParent(), a->getIndex(), b->getParent(), b->getIndex());
    branches_.push_back(branch);
    buildConnectivityGraph();
    solved_ = false;
    return branch;
}

// 断开指定端口的所有连接
void Circuit::disconnect(Port* port) {
    if (!port) {
        return;
    }

    const int componentId = port->getParent() ? port->getParent()->getId() : -1;
    const int portIndex = port->getIndex();
    // 删除与该端口关联的所有支路
    branches_.erase(std::remove_if(branches_.begin(), branches_.end(),
                                   [componentId, portIndex](Branch* branch) {
                                       if (!branch) {
                                           return true;
                                       }
                                       const bool matchStart = branch->getStartComponent() &&
                                           branch->getStartComponent()->getId() == componentId &&
                                           branch->getStartPortIndex() == portIndex;
                                       const bool matchEnd = branch->getEndComponent() &&
                                           branch->getEndComponent()->getId() == componentId &&
                                           branch->getEndPortIndex() == portIndex;
                                       if (matchStart || matchEnd) {
                                           delete branch;
                                           return true;
                                       }
                                       return false;
                                   }),
                      branches_.end());
    buildConnectivityGraph();
    solved_ = false;
}

// 构建连接图：根据支路关系建立节点和端口连接
void Circuit::buildConnectivityGraph() {
    std::vector<Port*> allPorts;
    std::map<PortKey, size_t> portIndices;
    std::map<PortKey, Port*> referencedPorts;

    // 收集所有被支路引用的端口
    for (const auto* branch : branches_) {
        if (!branch) {
            continue;
        }
        if (const Component* comp = branch->getStartComponent(); comp && branch->getStartPortIndex() >= 0) {
            PortKey key{comp->getId(), branch->getStartPortIndex()};
            if (!referencedPorts.count(key)) {
                if (Port* port = comp->getPort(static_cast<size_t>(branch->getStartPortIndex()))) {
                    referencedPorts[key] = port;
                }
            }
        }
        if (const Component* comp = branch->getEndComponent(); comp && branch->getEndPortIndex() >= 0) {
            PortKey key{comp->getId(), branch->getEndPortIndex()};
            if (!referencedPorts.count(key)) {
                if (Port* port = comp->getPort(static_cast<size_t>(branch->getEndPortIndex()))) {
                    referencedPorts[key] = port;
                }
            }
        }
    }

    // 断开所有元器件的端口连接
    for (const auto& [id, comp] : components_) {
        if (!comp) {
            continue;
        }
        const auto ports = comp->getPorts();
        for (Port* port : ports) {
            if (port) {
                port->disconnect();
            }
        }
    }

    // 建立端口到索引的映射
    for (const auto& [key, port] : referencedPorts) {
        if (!port) {
            continue;
        }
        portIndices[key] = allPorts.size();
        allPorts.push_back(port);
    }

    // 清空旧节点
    for (auto* node : nodes_) {
        delete node;
    }
    nodes_.clear();

    if (allPorts.empty()) {
        for (auto* branch : branches_) {
            if (branch) {
                branch->setNodeA(nullptr);
                branch->setNodeB(nullptr);
            }
        }
        return;
    }

    // 使用并查集将连通的端口归为同一节点
    UnionFind uf(static_cast<int>(allPorts.size()));
    auto lookupPort = [&portIndices](const Branch* branch, bool start) -> int {
        if (!branch) {
            return -1;
        }
        const Component* comp = start ? branch->getStartComponent() : branch->getEndComponent();
        const int portIndex = start ? branch->getStartPortIndex() : branch->getEndPortIndex();
        if (!comp || portIndex < 0) {
            return -1;
        }
        PortKey key{comp->getId(), portIndex};
        auto it = portIndices.find(key);
        return it == portIndices.end() ? -1 : static_cast<int>(it->second);
    };

    for (const auto* branch : branches_) {
        const int a = lookupPort(branch, true);
        const int b = lookupPort(branch, false);
        if (a >= 0 && b >= 0) {
            uf.unite(a, b);
        }
    }

    // 为每个连通分量创建节点
    std::map<int, Node*> rootToNode;
    for (size_t i = 0; i < allPorts.size(); ++i) {
        const int root = uf.find(static_cast<int>(i));
        Node* node = nullptr;
        auto it = rootToNode.find(root);
        if (it == rootToNode.end()) {
            node = new Node(nextNodeId_++);
            rootToNode[root] = node;
            nodes_.push_back(node);
        } else {
            node = it->second;
        }
        allPorts[i]->connectTo(node);
    }

    // 更新支路的节点引用
    for (auto* branch : branches_) {
        if (!branch) {
            continue;
        }
        Component* startComp = branch->getStartComponent();
        Component* endComp = branch->getEndComponent();
        branch->setNodeA(startComp && startComp->getPort(static_cast<size_t>(branch->getStartPortIndex()))
                            ? startComp->getPort(static_cast<size_t>(branch->getStartPortIndex()))->getNode()
                            : nullptr);
        branch->setNodeB(endComp && endComp->getPort(static_cast<size_t>(branch->getEndPortIndex()))
                            ? endComp->getPort(static_cast<size_t>(branch->getEndPortIndex()))->getNode()
                            : nullptr);
    }
}

// 执行电路仿真
bool Circuit::solve() {
    if (components_.empty()) {
        lastError_ = "No components in circuit";
        solved_ = false;
        return false;
    }

    // 检查是否有有效的连接
    bool hasConnections = false;
    for (const auto& [id, comp] : components_) {
        const auto ports = comp->getPorts();
        if (ports.size() >= 2 && ports[0] && ports[1] && ports[0]->isConnected() && ports[1]->isConnected()) {
            hasConnections = true;
            break;
        }
    }
    if (!hasConnections) {
        lastError_ = "Circuit is not connected";
        solved_ = false;
        return false;
    }

    const bool result = solver_->solve(components_, nodes_);
    solved_ = result;
    if (result) {
        simulation_results_ = getResults();
        lastError_.clear();
    } else {
        lastError_ = solver_->getLastError();
    }
    return result;
}

// 获取所有元器件的仿真结果
std::vector<SimulationResult> Circuit::getResults() const {
    std::vector<SimulationResult> results;
    results.reserve(components_.size());

    for (const auto& [id, comp] : components_) {
        SimulationResult r;
        r.componentId = comp->getId();
        r.componentName = comp->getName();
        r.componentType = ComponentFactory::typeToString(comp->getType());

        const auto props = comp->getAllProperties();
        r.voltage = props.count("voltage") ? props.at("voltage") : 0.0;
        r.current = props.count("current") ? props.at("current") : 0.0;
        r.power = props.count("power") ? props.at("power") : 0.0;
        r.isActive = props.count("isActive") ? props.at("isActive") != 0.0 : false;
        r.extra = props;
        results.push_back(r);
    }

    return results;
}

// 按ID查询仿真结果
SimulationResult Circuit::queryById(int id) const {
    for (const auto& result : getResults()) {
        if (result.componentId == id) {
            return result;
        }
    }
    return {};
}

// 按类型查询仿真结果
std::vector<SimulationResult> Circuit::queryByType(const std::string& type) const {
    std::vector<SimulationResult> filtered;
    for (const auto& result : getResults()) {
        if (result.componentType == type) {
            filtered.push_back(result);
        }
    }
    return filtered;
}

// 判断电路是否有效（至少有一个元器件）
bool Circuit::isValid() const {
    return !components_.empty();
}

// 设置ID计数器（反序列化时使用）
void Circuit::setIdCounters(int nextComponentId, int nextNodeId, int nextBranchId) {
    nextComponentId_ = std::max(1, nextComponentId);
    nextNodeId_ = std::max(1, nextNodeId);
    nextBranchId_ = std::max(1, nextBranchId);
}

// 清空电路所有内容
void Circuit::clear() {
    for (auto& [id, comp] : components_) {
        delete comp;
    }
    components_.clear();

    for (auto* node : nodes_) {
        delete node;
    }
    nodes_.clear();

    for (auto* branch : branches_) {
        delete branch;
    }
    branches_.clear();

    simulation_results_.clear();
    nextComponentId_ = 1;
    nextNodeId_ = 1;
    nextBranchId_ = 1;
    solved_ = false;
    lastError_.clear();
}

// 将电路序列化为JSON字符串
std::string Circuit::toJson() const {
    return CircuitJsonSerializer::serializeToJson(*this).dump(4);
}

// 从JSON字符串恢复电路
bool Circuit::fromJson(const std::string& jsonStr) {
    try {
        const auto j = json::parse(jsonStr);
        return CircuitJsonSerializer::deserializeFromJson(j, *this);
    } catch (...) {
        lastError_ = "Failed to parse JSON";
        return false;
    }
}

// 保存电路到JSON文件
bool Circuit::saveToJsonFile(const std::string& file_path) {
    return CircuitJsonSerializer::saveToJsonFile(*this, file_path);
}

// 从JSON文件加载电路
bool Circuit::loadFromJsonFile(const std::string& file_path) {
    return CircuitJsonSerializer::loadFromJsonFile(file_path, *this);
}

// 获取指定节点的电压
double Circuit::getNodeVoltage(int nodeId) const {
    for (auto* node : nodes_) {
        if (node && node->getId() == nodeId) {
            return node->getVoltage();
        }
    }
    return 0.0;
}

// 记录本次仿真到日志
void Circuit::logSimulation(const std::string& description) const {
    SimulationLogger::getInstance().logSimulation(getResults(), description);
}

// 验证电路合法性，返回错误和警告列表
std::vector<std::string> Circuit::validateCircuit() const {
    std::vector<std::string> errors;
    auto result = CircuitValidator::validate(components_, nodes_);
    if (!result.isValid) {
        errors.push_back(result.errorMessage);
    }
    for (const auto& warning : result.warnings) {
        errors.push_back("Warning: " + warning);
    }
    return errors;
}

} // namespace CircuitSim