#ifndef CIRCUIT_VALIDATOR_H
#define CIRCUIT_VALIDATOR_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Component.h"
#include "Node.h"

namespace CircuitSim {

class CircuitValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::string errorMessage;
        std::vector<std::string> warnings;

        ValidationResult() : isValid(true) {}
    };

    static ValidationResult validate(const std::unordered_map<int, Component*>& components,
                                     const std::vector<Node*>& nodes);

    static bool isConnected(const std::unordered_map<int, Component*>& components,
                            const std::vector<Node*>& nodes,
                            std::string& errorMsg);

    static bool noShortCircuit(const std::unordered_map<int, Component*>& components,
                               std::string& errorMsg);

    static bool validateComponents(const std::unordered_map<int, Component*>& components,
                                   std::vector<std::string>& warnings);
};

} // namespace CircuitSim

#endif
