#include "CircuitValidator.h"
#include "Port.h"
#include <set>
#include <queue>

namespace CircuitSim {

CircuitValidator::ValidationResult CircuitValidator::validate(
    const std::unordered_map<int, Component*>& components,
    const std::vector<Node*>& nodes) {
    
    ValidationResult result;
    
    if (components.empty()) {
        result.isValid = false;
        result.errorMessage = "电路中没有元件";
        return result;
    }
    
    if (nodes.empty()) {
        result.isValid = false;
        result.errorMessage = "电路中没有节点";
        return result;
    }
    
    if (!hasGround(components)) {
        result.isValid = false;
        result.errorMessage = "电路缺少接地元件（Ground）";
        return result;
    }
    
    std::string connError;
    if (!isConnected(components, nodes, connError)) {
        result.isValid = false;
        result.errorMessage = connError;
        return result;
    }
    
    std::string shortError;
    if (!noShortCircuit(components, shortError)) {
        result.isValid = false;
        result.errorMessage = shortError;
        return result;
    }
    
    validateComponents(components, result.warnings);
    
    return result;
}

bool CircuitValidator::hasGround(const std::unordered_map<int, Component*>& components) {
    for (const auto& pair : components) {
        if (pair.second->getType() == ComponentType::GROUND) {
            return true;
        }
    }
    return false;
}

std::unordered_map<int, std::vector<Node*>> CircuitValidator::buildAdjacency(
    const std::unordered_map<int, Component*>& components,
    const std::vector<Node*>& nodes) {
    
    std::unordered_map<int, std::vector<Node*>> adjacency;
    
    for (const auto& pair : components) {
        Component* comp = pair.second;
        auto& ports = comp->getPorts();
        if (ports.size() >= 2 && ports[0]->isConnected() && ports[1]->isConnected()) {
            Node* n1 = ports[0]->getNode();
            Node* n2 = ports[1]->getNode();
            adjacency[n1->getId()].push_back(n2);
            adjacency[n2->getId()].push_back(n1);
        }
    }
    
    return adjacency;
}

bool CircuitValidator::isConnected(const std::unordered_map<int, Component*>& components,
                                   const std::vector<Node*>& nodes,
                                   std::string& errorMsg) {
    if (nodes.empty()) return true;
    
    auto adjacency = buildAdjacency(components, nodes);
    
    std::set<int> connectedNodes;
    std::queue<Node*> q;
    
    q.push(nodes[0]);
    connectedNodes.insert(nodes[0]->getId());
    
    while (!q.empty()) {
        Node* current = q.front();
        q.pop();
        
        auto it = adjacency.find(current->getId());
        if (it != adjacency.end()) {
            for (Node* neighbor : it->second) {
                if (connectedNodes.find(neighbor->getId()) == connectedNodes.end()) {
                    connectedNodes.insert(neighbor->getId());
                    q.push(neighbor);
                }
            }
        }
    }
    
    std::vector<int> disconnectedNodes;
    for (Node* node : nodes) {
        if (connectedNodes.find(node->getId()) == connectedNodes.end()) {
            disconnectedNodes.push_back(node->getId());
        }
    }
    
    if (!disconnectedNodes.empty()) {
        errorMsg = "存在孤岛节点: ";
        for (size_t i = 0; i < disconnectedNodes.size(); ++i) {
            if (i > 0) errorMsg += ", ";
            errorMsg += std::to_string(disconnectedNodes[i]);
        }
        return false;
    }
    
    return true;
}

bool CircuitValidator::noShortCircuit(const std::unordered_map<int, Component*>& components,
                                      std::string& errorMsg) {
    std::vector<Component*> sources;
    for (const auto& pair : components) {
        if (pair.second->getType() == ComponentType::POWER_SOURCE) {
            sources.push_back(pair.second);
        }
    }
    
    for (size_t i = 0; i < sources.size(); ++i) {
        for (size_t j = i + 1; j < sources.size(); ++j) {
            Component* s1 = sources[i];
            Component* s2 = sources[j];
            
            auto& p1 = s1->getPorts();
            auto& p2 = s2->getPorts();
            
            if (p1.size() >= 2 && p2.size() >= 2) {
                bool p1_0_connected = p1[0]->isConnected();
                bool p1_1_connected = p1[1]->isConnected();
                bool p2_0_connected = p2[0]->isConnected();
                bool p2_1_connected = p2[1]->isConnected();
                
                if (p1_0_connected && p1_1_connected && p2_0_connected && p2_1_connected) {
                    Node* s1n0 = p1[0]->getNode();
                    Node* s1n1 = p1[1]->getNode();
                    Node* s2n0 = p2[0]->getNode();
                    Node* s2n1 = p2[1]->getNode();
                    
                    bool sameConnection0 = (s1n0 == s2n0 && s1n1 == s2n1);
                    bool sameConnection1 = (s1n0 == s2n1 && s1n1 == s2n0);
                    
                    if (sameConnection0 || sameConnection1) {
                        errorMsg = "电源直接短路: 元件 " + std::to_string(s1->getId()) 
                                  + " 和 " + std::to_string(s2->getId()) 
                                  + " 直接并联连接";
                        return false;
                    }
                }
            }
        }
    }
    
    return true;
}

bool CircuitValidator::validateComponents(const std::unordered_map<int, Component*>& components,
                                          std::vector<std::string>& warnings) {
    bool allValid = true;
    
    for (const auto& pair : components) {
        Component* comp = pair.second;
        
        switch (comp->getType()) {
            case ComponentType::RESISTOR: {
                double r = comp->getProperty("resistance");
                if (r <= 0) {
                    warnings.push_back("电阻 " + std::to_string(comp->getId()) 
                                     + " 的阻值无效: " + std::to_string(r) + " Ω");
                    allValid = false;
                }
                break;
            }
            
            case ComponentType::BULB: {
                double r = comp->getProperty("resistance");
                if (r <= 0) {
                    warnings.push_back("灯泡 " + std::to_string(comp->getId()) 
                                     + " 的电阻值无效: " + std::to_string(r) + " Ω");
                    allValid = false;
                }
                break;
            }
            
            case ComponentType::POWER_SOURCE: {
                double value = comp->getProperty("value");
                if (value <= 0) {
                    warnings.push_back("电源 " + std::to_string(comp->getId()) 
                                     + " 的值无效: " + std::to_string(value));
                    allValid = false;
                }
                break;
            }
            
            case ComponentType::CAPACITOR: {
                double c = comp->getProperty("capacitance");
                if (c <= 0) {
                    warnings.push_back("电容 " + std::to_string(comp->getId()) 
                                     + " 的电容值无效: " + std::to_string(c) + " F");
                    allValid = false;
                }
                break;
            }
            
            case ComponentType::INDUCTOR: {
                double l = comp->getProperty("inductance");
                if (l <= 0) {
                    warnings.push_back("电感 " + std::to_string(comp->getId()) 
                                     + " 的电感值无效: " + std::to_string(l) + " H");
                    allValid = false;
                }
                break;
            }
            
            default:
                break;
        }
    }
    
    return allValid;
}

} // namespace CircuitSim