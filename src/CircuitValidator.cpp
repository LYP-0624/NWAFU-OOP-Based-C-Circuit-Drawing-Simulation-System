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

CircuitValidator::ValidationResult CircuitValidator::validate(
    const std::unordered_map<int, Component*>& components,
    const std::vector<Node*>& nodes) {

    ValidationResult result;

    if (components.empty()) {
        result.isValid = false;
        result.errorMessage = "Circuit has no components";
        return result;
    }
    if (nodes.empty()) {
        result.isValid = false;
        result.errorMessage = "Circuit has no nodes";
        return result;
    }

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

bool CircuitValidator::isConnected(const std::unordered_map<int, Component*>& components,
                                   const std::vector<Node*>& nodes,
                                   std::string& errorMsg) {
    if (nodes.empty()) {
        return true;
    }

    for (const auto& [id, comp] : components) {
        const auto ports = comp->getPorts();
        if (ports.size() < 2 || !ports[0] || !ports[1] || !ports[0]->isConnected() || !ports[1]->isConnected()) {
            errorMsg = "Component " + std::to_string(id) + " is not fully connected";
            return false;
        }
        if (ports[0]->getNode() == ports[1]->getNode()) {
            errorMsg = "Component " + std::to_string(id) + " is shorted on the same node";
            return false;
        }
    }

    return true;
}

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

        if (ports[0]->getNode() == ports[1]->getNode()) {
            errorMsg = "Voltage source " + std::to_string(id) + " is shorted";
            return false;
        }
    }

    return true;
}

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
