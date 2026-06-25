#ifndef CIRCUIT_VALIDATOR_H
#define CIRCUIT_VALIDATOR_H

#include <string>
#include <unordered_map>
#include <vector>

#include "Component.h"
#include "Node.h"

namespace CircuitSim {

// 电路验证器
class CircuitValidator {
public:
    // 验证结果结构
    struct ValidationResult {
        bool isValid;           // 是否有效
        std::string errorMessage; // 错误信息
        std::vector<std::string> warnings; // 警告列表

        ValidationResult() : isValid(true) {}
    };

    static ValidationResult validate(const std::unordered_map<int, Component*>& components,
                                     const std::vector<Node*>& nodes); // 验证电路

    static bool isConnected(const std::unordered_map<int, Component*>& components,
                            const std::vector<Node*>& nodes,
                            std::string& errorMsg); // 检查电路是否连通

    static bool noShortCircuit(const std::unordered_map<int, Component*>& components,
                               std::string& errorMsg); // 检查是否存在短路

    static bool validateComponents(const std::unordered_map<int, Component*>& components,
                                   std::vector<std::string>& warnings); // 验证元件
};

} // namespace CircuitSim

#endif
