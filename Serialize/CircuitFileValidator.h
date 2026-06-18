#ifndef CIRCUIT_FILE_VALIDATOR_H
#define CIRCUIT_FILE_VALIDATOR_H

#include <string>

#include "json.hpp"

using json = nlohmann::json;

namespace CircuitSim {

class CircuitFileValidator {
public:
    static bool validateFilePath(const std::string& file_path, bool for_writing);
    static bool validateJsonFileFormat(const std::string& file_path);
    static bool validateComponentParameters(const json& comp_json);
};

} // namespace CircuitSim

#endif // CIRCUIT_FILE_VALIDATOR_H
