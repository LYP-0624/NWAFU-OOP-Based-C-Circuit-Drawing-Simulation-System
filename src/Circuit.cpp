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

struct UnionFind {
    std::vector<int> parent;
    std::vector<int> rank;

    explicit UnionFind(int size) : parent(size), rank(size, 0) {
        for (int i = 0; i < size; ++i) {
            parent[i] = i;
        }
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

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

struct PortKey {
    int componentId = -1;
    int portIndex = -1;

    bool operator<(const PortKey& other) const {
        if (componentId != other.componentId) {
            return componentId < other.componentId;
        }
        return portIndex < other.portIndex;
    }
};

} // namespace

Circuit::Circuit()
    : nextComponentId_(1), nextNodeId_(1), nextBranchId_(1), solved_(false), solver_(new MNASolver()) {}

Circuit::~Circuit() {
    clear();
    delete solver_;
}

void Circuit::addComponent(Component* comp) {
    if (!comp) {
        return;
    }
    components_[comp->getId()] = comp;
    nextComponentId_ = std::max(nextComponentId_, comp->getId() + 1);
    solved_ = false;
}

void Circuit::removeComponent(int id) {
    auto it = components_.find(id);
    if (it == components_.end()) {
        return;
    }

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
    buildConnectivityGraph();
    solved_ = false;
}

Component* Circuit::getComponent(int id) const {
    auto it = components_.find(id);
    return it == components_.end() ? nullptr : it->second;
}

Branch* Circuit::getBranch(int id) const {
    for (auto* branch : branches_) {
        if (branch && branch->getId() == id) {
            return branch;
        }
    }
    return nullptr;
}

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

void Circuit::disconnect(Port* port) {
    if (!port) {
        return;
    }

    const int componentId = port->getParent() ? port->getParent()->getId() : -1;
    const int portIndex = port->getIndex();
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

void Circuit::buildConnectivityGraph() {
    std::vector<Port*> allPorts;
    std::map<PortKey, size_t> portIndices;
    std::map<PortKey, Port*> referencedPorts;

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

    for (const auto& [key, port] : referencedPorts) {
        if (!port) {
            continue;
        }
        portIndices[key] = allPorts.size();
        allPorts.push_back(port);
    }

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

bool Circuit::solve() {
    if (components_.empty()) {
        lastError_ = "No components in circuit";
        solved_ = false;
        return false;
    }

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

SimulationResult Circuit::queryById(int id) const {
    for (const auto& result : getResults()) {
        if (result.componentId == id) {
            return result;
        }
    }
    return {};
}

std::vector<SimulationResult> Circuit::queryByType(const std::string& type) const {
    std::vector<SimulationResult> filtered;
    for (const auto& result : getResults()) {
        if (result.componentType == type) {
            filtered.push_back(result);
        }
    }
    return filtered;
}

bool Circuit::isValid() const {
    return !components_.empty();
}

void Circuit::setIdCounters(int nextComponentId, int nextNodeId, int nextBranchId) {
    nextComponentId_ = std::max(1, nextComponentId);
    nextNodeId_ = std::max(1, nextNodeId);
    nextBranchId_ = std::max(1, nextBranchId);
}

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

std::string Circuit::toJson() const {
    return CircuitJsonSerializer::serializeToJson(*this).dump(4);
}

bool Circuit::fromJson(const std::string& jsonStr) {
    try {
        const auto j = json::parse(jsonStr);
        return CircuitJsonSerializer::deserializeFromJson(j, *this);
    } catch (...) {
        lastError_ = "Failed to parse JSON";
        return false;
    }
}

bool Circuit::saveToJsonFile(const std::string& file_path) {
    return CircuitJsonSerializer::saveToJsonFile(*this, file_path);
}

bool Circuit::loadFromJsonFile(const std::string& file_path) {
    return CircuitJsonSerializer::loadFromJsonFile(file_path, *this);
}

double Circuit::getNodeVoltage(int nodeId) const {
    for (auto* node : nodes_) {
        if (node && node->getId() == nodeId) {
            return node->getVoltage();
        }
    }
    return 0.0;
}

void Circuit::logSimulation(const std::string& description) const {
    SimulationLogger::getInstance().logSimulation(getResults(), description);
}

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
