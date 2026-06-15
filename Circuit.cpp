#include "Circuit.h"
#include "Component.h"
#include "Port.h"
#include "Node.h"
#include "LinearSolver.h"
#include <sstream>
#include <cmath>

namespace CircuitSim {

Circuit::Circuit() : nextComponentId_(1), nextNodeId_(1), solved_(false) {}

Circuit::~Circuit() {
    clear();
}

void Circuit::addComponent(Component* comp) {
    if (comp) {
        components_[comp->getId()] = comp;
        if (comp->getId() >= nextComponentId_) {
            nextComponentId_ = comp->getId() + 1;
        }
    }
}

void Circuit::removeComponent(int id) {
    auto it = components_.find(id);
    if (it != components_.end()) {
        for (auto* p : it->second->getPorts()) {
            disconnect(p);
        }
        delete it->second;
        components_.erase(it);
    }
}

Component* Circuit::getComponent(int id) const {
    auto it = components_.find(id);
    return (it != components_.end()) ? it->second : nullptr;
}

Node* Circuit::createNode() {
    Node* node = new Node(nextNodeId_++);
    nodes_.push_back(node);
    return node;
}

void Circuit::connect(Port* a, Port* b) {
    if (!a || !b || a == b) return;
    
    if (!a->isConnected() && !b->isConnected()) {
        Node* node = createNode();
        a->connectTo(node);
        b->connectTo(node);
    } else if (a->isConnected() && !b->isConnected()) {
        b->connectTo(a->getNode());
    } else if (!a->isConnected() && b->isConnected()) {
        a->connectTo(b->getNode());
    } else {
        mergeNodes(a->getNode(), b->getNode());
    }
}

void Circuit::disconnect(Port* port) {
    if (port) {
        port->disconnect();
    }
}

void Circuit::mergeNodes(Node* a, Node* b) {
    if (!a || !b || a == b) return;
    a->merge(b);
    for (auto it = nodes_.begin(); it != nodes_.end(); ++it) {
        if (*it == b) {
            delete *it;
            nodes_.erase(it);
            break;
        }
    }
}

bool Circuit::solve() {
    if (components_.empty()) return false;
    
    int n = static_cast<int>(nodes_.size());
    if (n == 0) return false;
    
    std::vector<std::vector<double>> A(n, std::vector<double>(n, 0.0));
    std::vector<double> b(n, 0.0);
    std::vector<double> x;
    
    // 简化：节点电压法（仅处理电阻+电压源）
    // 实际应使用改进节点分析法(MNA)
    for (auto& [id, comp] : components_) {
        if (comp->getType() == ComponentType::POWER_SOURCE) {
            double v = comp->getProperty("voltage");
            // 找到电源连接的节点，设置电压
            for (auto* p : comp->getPorts()) {
                if (p->isConnected()) {
                    Node* node = p->getNode();
                    int idx = node->getId() - 1;
                    if (idx >= 0 && idx < n) {
                        A[idx][idx] = 1.0;
                        b[idx] = v;
                    }
                }
            }
        }
    }
    
    // 填充电阻的导纳
    for (auto& [id, comp] : components_) {
        if (comp->getType() == ComponentType::RESISTOR || comp->getType() == ComponentType::BULB) {
            double r = comp->getProperty("resistance");
            if (r < 1e-10) continue;
            double g = 1.0 / r;
            
            auto& ports = comp->getPorts();
            if (ports.size() >= 2 && ports[0]->isConnected() && ports[1]->isConnected()) {
                int n1 = ports[0]->getNode()->getId() - 1;
                int n2 = ports[1]->getNode()->getId() - 1;
                if (n1 >= 0 && n1 < n && n2 >= 0 && n2 < n) {
                    A[n1][n1] += g;
                    A[n2][n2] += g;
                    A[n1][n2] -= g;
                    A[n2][n1] -= g;
                }
            }
        }
    }
    
    if (!LinearSolver::solve(A, b, x)) return false;
    
    for (size_t i = 0; i < nodes_.size() && i < x.size(); ++i) {
        nodes_[i]->setVoltage(x[i]);
    }
    
    // 更新各元器件状态
    for (auto& [id, comp] : components_) {
        auto& ports = comp->getPorts();
        double v = 0.0, i = 0.0;
        if (ports.size() >= 2 && ports[0]->isConnected() && ports[1]->isConnected()) {
            v = std::abs(ports[0]->getVoltage() - ports[1]->getVoltage());
            if (comp->getType() == ComponentType::RESISTOR || comp->getType() == ComponentType::BULB) {
                double r = comp->getProperty("resistance");
                if (r > 1e-10) i = v / r;
            }
        }
        comp->simulate(v, i);
    }
    
    solved_ = true;
    return true;
}

std::vector<<SimulationResult> Circuit::getResults() const {
    std::vector<<SimulationResult> results;
    for (const auto& [id, comp] : components_) {
        SimulationResult r;
        r.componentId = comp->getId();
        r.componentName = comp->getName();
        r.componentType = ComponentFactory::typeToString(comp->getType());
        
        auto props = comp->getAllProperties();
        r.voltage = props.count("voltage") ? props["voltage"] : 0.0;
        r.current = props.count("current") ? props["current"] : 0.0;
        r.power = props.count("power") ? props["power"] : 0.0;
        r.isActive = props.count("isActive") ? static_cast<bool>(props["isActive"]) : false;
        
        if (comp->getType() == ComponentType::AMMETER) {
            r.extra["reading"] = props.count("current") ? props["current"] : 0.0;
        } else if (comp->getType() == ComponentType::VOLTMETER) {
            r.extra["reading"] = props.count("voltage") ? props["voltage"] : 0.0;
        }
        
        results.push_back(r);
    }
    return results;
}

SimulationResult Circuit::queryById(int id) const {
    auto it = components_.find(id);
    if (it != components_.end()) {
        auto results = getResults();
        for (const auto& r : results) {
            if (r.componentId == id) return r;
        }
    }
    return SimulationResult{};
}

std::vector<<SimulationResult> Circuit::queryByType(const std::string& type) const {
    std::vector<<SimulationResult> filtered;
    auto results = getResults();
    for (const auto& r : results) {
        if (r.componentType == type) filtered.push_back(r);
    }
    return filtered;
}

bool Circuit::isValid() const {
    return !components_.empty() && hasGround();
}

bool Circuit::hasGround() const {
    for (auto* node : nodes_) {
        if (node->isGround()) return true;
    }
    return false;
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
    nextComponentId_ = 1;
    nextNodeId_ = 1;
    solved_ = false;
}

std::string Circuit::toJson() const {
    std::stringstream ss;
    ss << "{\"version\":\"1.0\",\"components\":[";
    bool first = true;
    for (const auto& [id, comp] : components_) {
        if (!first) ss << ",";
        first = false;
        ss << "{\"id\":" << id << ",\"type\":\"" << ComponentFactory::typeToString(comp->getType())
           << "\",\"x\":" << comp->getX() << ",\"y\":" << comp->getY() << "}";
    }
    ss << "],\"connections\":[]}";
    return ss.str();
}

bool Circuit::fromJson(const std::string& json) {
    // 简化实现，实际应使用JSON库
    clear();
    return true;
}

} // namespace CircuitSim