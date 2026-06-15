#ifndef CIRCUIT_FILE_VALIDATOR_H
#define CIRCUIT_FILE_VALIDATOR_H

#include <string>
#include <nlohmann/json.hpp>
#include "SimulationLogger.h"

using json = nlohmann::json;

namespace CircuitSim
{
    class CircuitFileValidator
    {
    public:
        // 校验文件路径合法性
        static bool validateFilePath(const std::string& file_path, bool for_writing);
        
        // 校验JSON文件格式合法性
        static bool validateJsonFileFormat(const std::string& file_path);
        
        // 校验元件参数合法性
        static bool validateComponentParameters(const json& comp_json);
        
        // 校验电路拓扑合法性
        static bool validateCircuitTopology(const json& json_data);
    };
}

#endif // CIRCUIT_FILE_VALIDATOR_H
