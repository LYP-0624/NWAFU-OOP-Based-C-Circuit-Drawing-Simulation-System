#ifndef CIRCUIT_JSON_SERIALIZER_H
#define CIRCUIT_JSON_SERIALIZER_H

#include <string>

#include "json.hpp"

#include "Branch.h"
#include "Circuit.h"
#include "CircuitException.h"
#include "Node.h"
#include "SimulationLogger.h"

using json = nlohmann::json;

namespace CircuitSim {

class CircuitJsonSerializer {
public:
    static json serializeToJson(const Circuit& circuit);
    static bool deserializeFromJson(const json& json_data, Circuit& circuit);
    static bool saveToJsonFile(const Circuit& circuit, const std::string& file_path);
    static bool loadFromJsonFile(const std::string& file_path, Circuit& circuit);

private:
    static bool validateJsonRootStructure(const json& json_data);
    static bool validateComponentJson(const json& comp_json);

    static json serializeComponent(const Component* component);
    static json serializeNode(const Node* node);
    static json serializeBranch(const Branch* branch);

    static Component* deserializeComponent(const json& comp_json);
    static Node* deserializeNode(const json& node_json);
    static Branch* deserializeBranch(const json& branch_json, Circuit& circuit);
};

} // namespace CircuitSim

#endif // CIRCUIT_JSON_SERIALIZER_H
