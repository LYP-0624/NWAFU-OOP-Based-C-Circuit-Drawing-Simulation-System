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
        // Validate file path legality (checks extension, existence, permissions)
        static bool validateFilePath(const std::string& file_path, bool for_writing);
        
        // Validate JSON file format (checks file header, size)
        static bool validateJsonFileFormat(const std::string& file_path);
        
        // Validate component parameters legality and range
        static bool validateComponentParameters(const json& comp_json);
        
        // Validate circuit topology (checks for power source, ground, etc.)
        static bool validateCircuitTopology(const json& json_data);
    };
}

#endif // CIRCUIT_FILE_VALIDATOR_H