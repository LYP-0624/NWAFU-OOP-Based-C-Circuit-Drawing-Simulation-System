#include "Circuit.h"
#include "Component.h"
#include "Port.h"
#include "Node.h"
#include "MNASolver.h"
#include "LinearSolver.h"
#include "CircuitValidator.h"
#include "SimulationLogger.h"
#include "ComponentFactory.h"
#include <sstream>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <set>
#include "../Serialize/CircuitJsonSerializer.h"

namespace CircuitSim {

Circuit::Circuit() : nextComponentId_(1), nextNodeId_(1), nextBranchId_(1), 
                     solved_(false), solver_(new MNASolver()) {}

Circuit::~Circuit() {
    clear();
    delete solver_;
}

void Circuit::addComponent(Component* comp) {
    if (comp) {
        components_[comp->getId()] = comp;
        if (comp->getId() >= nextComponentId_) {
            nextComponentId_ = comp->getId() + 1;
        }
        solved_ = false;
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
        solved_ = false;
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
    solved_ = false;
}

void Circuit::disconnect(Port* port) {
    if (port) {
        port->disconnect();
        solved_ = false;
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
    solved_ = false;
}

void Circuit::buildConnectivityGraph() {
    for (auto* node : nodes_) {
        if (node && node->getComponentCount() > 0) {
        }
    }
}

void Circuit::assignGroundNode() {
    for (const auto& [id, comp] : components_) {
        if (comp->getType() == ComponentType::GROUND) {
            auto& ports = comp->getPorts();
            Node* groundNode = nullptr;

            for (auto* port : ports) {
                if (port->isConnected() && port->getNode()) {
                    if (!groundNode) {
                        groundNode = port->getNode();
                        groundNode->setGround(true);
                    } else if (port->getNode() != groundNode) {
                        mergeNodes(groundNode, port->getNode());
                    }
                }
            }
            if (groundNode) return;
        }
    }

    if (nodes_.empty()) return;

    bool hasGround = false;
    for (auto* node : nodes_) {
        if (node->isGround()) {
            hasGround = true;
            break;
        }
    }

    if (!hasGround && !nodes_.empty()) {
        nodes_[0]->setGround(true);
    }
}

void Circuit::rebuildBranches() {
    for (auto* branch : branches_) {
        delete branch;
    }
    branches_.clear();

    std::set<std::pair<Node*, Node*>> existingPairs;

    for (const auto& [id, comp] : components_) {
        auto& ports = comp->getPorts();
        if (ports.size() >= 2 && ports[0]->isConnected() && ports[1]->isConnected()) {
            Node* n1 = ports[0]->getNode();
            Node* n2 = ports[1]->getNode();
            
            if (n1 && n2 && n1 != n2) {
                std::pair<Node*, Node*> pair1 = {n1, n2};
                std::pair<Node*, Node*> pair2 = {n2, n1};
                
                if (existingPairs.find(pair1) == existingPairs.end() &&
                    existingPairs.find(pair2) == existingPairs.end()) {
                    Branch* branch = new Branch(nextBranchId_++, comp, n1, n2);
                    branches_.push_back(branch);
                    existingPairs.insert(pair1);
                }
            }
        }
    }
}

bool Circuit::solve() {
    if (components_.empty() || nodes_.empty()) {
        return false;
    }

    assignGroundNode();
    buildConnectivityGraph();
    rebuildBranches();

    bool result = solver_->solve(components_, nodes_);
    solved_ = result;

    if (result) {
        for (auto* branch : branches_) {
            Component* comp = branch->getComponent();
            if (!comp) comp = branch->getStartComponent();
            if (comp) {
                branch->setCurrent(comp->getProperty("current"));
            }
        }
        simulation_results_ = getResults();
    }

    return result;
}

std::vector<SimulationResult> Circuit::getResults() const {
    std::vector<SimulationResult> results;
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
        } else if (comp->getType() == ComponentType::BULB) {
            r.extra["isLit"] = props.count("isActive") ? props["isActive"] : 0.0;
        } else if (comp->getType() == ComponentType::SWITCH) {
            r.extra["state"] = props.count("state") ? props["state"] : 0.0;
        } else if (comp->getType() == ComponentType::CAPACITOR) {
            r.extra["capacitance"] = props.count("capacitance") ? props["capacitance"] : 0.0;
            r.extra["charge"] = props.count("charge") ? props["charge"] : 0.0;
        } else if (comp->getType() == ComponentType::INDUCTOR) {
            r.extra["inductance"] = props.count("inductance") ? props["inductance"] : 0.0;
        } else if (comp->getType() == ComponentType::GROUND) {
            r.extra["ground"] = 1.0;
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

std::vector<SimulationResult> Circuit::queryByType(const std::string& type) const {
    std::vector<SimulationResult> filtered;
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
    
    for (auto* branch : branches_) {
        delete branch;
    }
    branches_.clear();
    
    nextComponentId_ = 1;
    nextNodeId_ = 1;
    nextBranchId_ = 1;
    solved_ = false;
}

std::string Circuit::toJson() const {
    json j = CircuitJsonSerializer::serializeToJson(*this);
    return j.dump(4);
}

bool Circuit::fromJson(const std::string& jsonStr) {
    try {
        json j = json::parse(jsonStr);
        return CircuitJsonSerializer::deserializeFromJson(j, *this);
    } catch (...) {
        return false;
    }
}

bool Circuit::saveToJsonFile(const std::string& file_path) {
    rebuildBranches();
    return CircuitJsonSerializer::saveToJsonFile(*this, file_path);
}

bool Circuit::loadFromJsonFile(const std::string& file_path) {
    return CircuitJsonSerializer::loadFromJsonFile(file_path, *this);
}

const std::vector<Branch*>& Circuit::getBranchList() const {
    return branches_;
}

double Circuit::getNodeVoltage(int nodeId) const {
    for (auto* node : nodes_) {
        if (node->getId() == nodeId) {
            return node->getVoltage();
        }
    }
    return 0.0;
}

void Circuit::logSimulation(const std::string& description) const {
    auto results = getResults();
    SimulationLogger::getInstance().logSimulation(results, description);
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