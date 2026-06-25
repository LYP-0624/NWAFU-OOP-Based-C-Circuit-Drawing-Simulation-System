#include "CircuitValidator.h"

#include "Ammeter.h"
#include "Bulb.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "Port.h"
#include "Resistor.h"
#include "PowerSource.h"
#include "Switch.h"

namespace CircuitSim {

// 验证电路合法性：检查元器件、节点、电源、连接和短路等
CircuitValidator::ValidationResult CircuitValidator::validate(
    const std::unordered_map<int, Component*>& components,
    const std::vector<Node*>& nodes) {

    ValidationResult result;

    // 检查是否有元器件
    if (components.empty()) {
        result.isValid = false;
        result.errorMessage = "Circuit has no components";
        return result;
    }
    // 检查是否有节点
    if (nodes.empty()) {
        result.isValid = false;
        result.errorMessage = "Circuit has no nodes";
        return result;
    }

    // 检查是否至少有一个电源
    bool hasSource = false;
    for (const auto& [id, comp] : components) {
        if (comp->getType() == ComponentType::POWER_SOURCE) {
            hasSource = true;
            break;
        }
    }
    if (!hasSource) {
        result.isValid = false;
        result.errorMessage = "Circuit requires at least one power source";
        return result;
    }

    // 检查电路连通性
    std::string connError;
    if (!isConnected(components, nodes, connError)) {
        result.isValid = false;
        result.errorMessage = connError;
        return result;
    }

    // 检查是否存在短路
    std::string shortError;
    if (!noShortCircuit(components, shortError)) {
        result.isValid = false;
        result.errorMessage = shortError;
        return result;
    }

    // 验证各元器件参数合法性，收集警告信息
    validateComponents(components, result.warnings);
    return result;
}

// 检查所有元器件是否都已正确连接（两端端口均连接到不同节点）
bool CircuitValidator::isConnected(const std::unordered_map<int, Component*>& components,
                                   const std::vector<Node*>& nodes,
                                   std::string& errorMsg) {
    if (nodes.empty()) {
        return true;
    }

    for (const auto& [id, comp] : components) {
        const auto ports = comp->getPorts();
        // 检查元器件是否有至少两个端口且均已连接
        if (ports.size() < 2 || !ports[0] || !ports[1] || !ports[0]->isConnected() || !ports[1]->isConnected()) {
            errorMsg = "Component " + std::to_string(id) + " is not fully connected";
            return false;
        }
        // 检查同一元器件两端是否短接到同一节点
        if (ports[0]->getNode() == ports[1]->getNode()) {
            errorMsg = "Component " + std::to_string(id) + " is shorted on the same node";
            return false;
        }
    }

    return true;
}

// 检查电源是否被短路（两端连接到同一节点）
bool CircuitValidator::noShortCircuit(const std::unordered_map<int, Component*>& components,
                                      std::string& errorMsg) {
    for (const auto& [id, comp] : components) {
        if (comp->getType() != ComponentType::POWER_SOURCE) {
            continue;
        }

        const auto ports = comp->getPorts();
        if (ports.size() < 2 || !ports[0] || !ports[1] || !ports[0]->isConnected() || !ports[1]->isConnected()) {
            continue;
        }

        // 电源两端不能连到同一节点
        if (ports[0]->getNode() == ports[1]->getNode()) {
            errorMsg = "Voltage source " + std::to_string(id) + " is shorted";
            return false;
        }
    }

    return true;
}

// 验证各元器件参数是否合法（电阻、电容、电感、电源值等为正或非零）
bool CircuitValidator::validateComponents(const std::unordered_map<int, Component*>& components,
                                          std::vector<std::string>& warnings) {
    bool allValid = true;

    for (const auto& [id, comp] : components) {
        switch (comp->getType()) {
            case ComponentType::RESISTOR: {
                const double r = static_cast<Resistor*>(comp)->getResistance();
                if (r <= 0.0) {
                    warnings.push_back("Resistor " + std::to_string(id) + " has invalid resistance");
                    allValid = false;
                }
                break;
            }
            case ComponentType::BULB: {
                const double r = static_cast<Bulb*>(comp)->getResistance();
                if (r <= 0.0) {
                    warnings.push_back("Bulb " + std::to_string(id) + " has invalid resistance");
                    allValid = false;
                }
                break;
            }
            case ComponentType::POWER_SOURCE: {
                const double value = static_cast<PowerSource*>(comp)->getValue();
                if (value == 0.0) {
                    warnings.push_back("Power source " + std::to_string(id) + " has zero value");
                    allValid = false;
                }
                break;
            }
            case ComponentType::CAPACITOR: {
                const double c = static_cast<Capacitor*>(comp)->getCapacitance();
                if (c <= 0.0) {
                    warnings.push_back("Capacitor " + std::to_string(id) + " has invalid capacitance");
                    allValid = false;
                }
                break;
            }
            case ComponentType::INDUCTOR: {
                const double l = static_cast<Inductor*>(comp)->getInductance();
                if (l <= 0.0) {
                    warnings.push_back("Inductor " + std::to_string(id) + " has invalid inductance");
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