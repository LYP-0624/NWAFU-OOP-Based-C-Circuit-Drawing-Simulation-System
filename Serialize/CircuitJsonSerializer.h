#ifndef CIRCUIT_JSON_SERIALIZER_H
#define CIRCUIT_JSON_SERIALIZER_H

#include <string>
#include <nlohmann/json.hpp>
#include "Circuit.h"
#include "CircuitException.h"
#include "SimulationLogger.h"
#include "Branch.h"
#include "Node.h"

using json = nlohmann::json;

namespace CircuitSim
{
    class CircuitJsonSerializer
    {
    public:
        // Serialize circuit object to standard JSON
        static json serializeToJson(const Circuit& circuit);
        
        // Deserialize standard JSON to circuit object
        static bool deserializeFromJson(const json& json_data, Circuit& circuit);
        
        // Save circuit data to a local JSON file
        static bool saveToJsonFile(const Circuit& circuit, const std::string& file_path);
        
        // Load circuit from a local JSON file
        static bool loadFromJsonFile(const std::string& file_path, Circuit& circuit);

    private:
        // Validate JSON root structure
        static bool validateJsonRootStructure(const json& json_data);
        
        // Validate individual component JSON structure and parameters
        static bool validateComponentJson(const json& comp_json);
        
        // Validate node and connection relation legality
        static bool validateConnectionRelations(const json& json_data);
        
        // Component object to JSON
        static json serializeComponent(const Component* component);
        
        // Node object to JSON
        static json serializeNode(const Node* node);
        
        // Branch/connection object to JSON
        static json serializeBranch(const Branch* branch);
        
        // JSON to component object
        static Component* deserializeComponent(const json& comp_json);
        
        // JSON to node object
        static Node* deserializeNode(const json& node_json);
        
        // JSON to branch/connection object
        static Branch* deserializeBranch(const json& branch_json, Circuit& circuit);
    };
}

#endif // CIRCUIT_JSON_SERIALIZER_H