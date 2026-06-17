#include "CircuitJsonSerializer.h"
#include "ComponentFactory.h"
#include "CircuitFileValidator.h"
#include "Port.h"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <unordered_set>
#include <chrono>

namespace fs = std::filesystem;

namespace CircuitSim
{
    json CircuitJsonSerializer::serializeToJson(const Circuit& circuit)
    {
        SimulationLogger::getInstance().logInfo("Starting circuit JSON serialization");
        json root;

        // 1. Circuit metadata
        root["metadata"]["version"] = "1.0.0";
        root["metadata"]["circuit_name"] = circuit.getName();
        root["metadata"]["created_at"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        root["metadata"]["component_count"] = circuit.getComponents().size();
        root["metadata"]["node_count"] = circuit.getNodes().size();
        root["metadata"]["branch_count"] = circuit.getBranches().size();
        root["metadata"]["is_solved"] = circuit.isSolved();

        // 2. Serialize all components
        json components_array = json::array();
        for (const auto& [id, component] : circuit.getComponents())
        {
            components_array.push_back(serializeComponent(component));
        }
        root["components"] = components_array;

        // 3. Serialize all nodes
        json nodes_array = json::array();
        for (const auto& node : circuit.getNodes())
        {
            nodes_array.push_back(serializeNode(node));
        }
        root["nodes"] = nodes_array;

        // 4. Serialize all branches/connections
        json branches_array = json::array();
        for (const auto& branch : circuit.getBranches())
        {
            branches_array.push_back(serializeBranch(branch));
        }
        root["branches"] = branches_array;

        // 5. Serialize simulation results (if solved)
        if (circuit.isSolved())
        {
            json results_array = json::array();
            for (const auto& result : circuit.getSimulationResults())
            {
                json res_json;
                res_json["component_id"] = result.componentId;
                res_json["component_name"] = result.componentName;
                res_json["voltage"] = result.voltage;
                res_json["current"] = result.current;
                res_json["power"] = result.power;
                results_array.push_back(res_json);
            }
            root["simulation_results"] = results_array;
        }

        SimulationLogger::getInstance().logInfo("Circuit JSON serialization completed successfully");
        return root;
    }

    bool CircuitJsonSerializer::deserializeFromJson(const json& json_data, Circuit& circuit)
    {
        SimulationLogger::getInstance().logInfo("Starting circuit JSON deserialization");

        // 1. Validate root structure
        if (!validateJsonRootStructure(json_data))
        {
            SimulationLogger::getInstance().logError("JSON root structure validation failed");
            throw InvalidJsonException("Invalid JSON root structure");
            return false;
        }

        // 2. Clear existing circuit data
        circuit.clear();
        SimulationLogger::getInstance().logInfo("Existing circuit data cleared for deserialization");

        // 3. Deserialize nodes first (before components and connections)
        if (json_data.contains("nodes"))
        {
            for (const auto& node_json : json_data["nodes"])
            {
                Node* node = deserializeNode(node_json);
                if (node)
                {
                    circuit.addNode(node);
                    SimulationLogger::getInstance().logDebug("Node deserialized: ID=" + std::to_string(node->getId()));
                }
            }
        }

        // 4. Deserialize components
        if (json_data.contains("components"))
        {
            for (const auto& comp_json : json_data["components"])
            {
                if (!validateComponentJson(comp_json))
                {
                    SimulationLogger::getInstance().logWarn("Invalid component JSON, skipped");
                    continue;
                }

                Component* component = deserializeComponent(comp_json);
                if (component)
                {
                    circuit.addComponent(component);
                    SimulationLogger::getInstance().logInfo("Component deserialized: ID=" + std::to_string(component->getId()) + ", Type=" + component->getTypeName());
                }
            }
        }

        // 5. Deserialize branches/connections
        if (json_data.contains("branches"))
        {
            for (const auto& branch_json : json_data["branches"])
            {
                Branch* branch = deserializeBranch(branch_json, circuit);
                if (branch)
                {
                    circuit.addBranch(branch);
                    SimulationLogger::getInstance().logDebug("Branch deserialized: ID=" + std::to_string(branch->getId()));
                }
            }
        }

        // 6. Validate connection relations
        if (!validateConnectionRelations(json_data))
        {
            SimulationLogger::getInstance().logError("Connection relations validation failed");
            throw InvalidConnectionException("Invalid connection relations in JSON");
            return false;
        }

        // 7. Restore circuit metadata
        if (json_data["metadata"].contains("circuit_name"))
        {
            circuit.setName(json_data["metadata"]["circuit_name"]);
        }

        SimulationLogger::getInstance().logInfo("Circuit JSON deserialization completed successfully");
        return true;
    }

    bool CircuitJsonSerializer::saveToJsonFile(const Circuit& circuit, const std::string& file_path)
    {
        SimulationLogger::getInstance().logInfo("Saving circuit to JSON file: " + file_path);

        // 1. Validate file path
        if (!CircuitFileValidator::validateFilePath(file_path, true))
        {
            SimulationLogger::getInstance().logError("Invalid file path for saving");
            return false;
        }

        // 2. Serialize circuit to JSON
        json circuit_json = serializeToJson(circuit);

        // 3. Write to file
        std::ofstream out_file(file_path);
        if (!out_file.is_open())
        {
            SimulationLogger::getInstance().logError("Failed to open file for writing: " + file_path);
            throw FileIOException("Failed to open file for writing: " + file_path);
            return false;
        }

        out_file << circuit_json.dump(4);
        out_file.close();

        SimulationLogger::getInstance().logInfo("Circuit saved to JSON file successfully: " + file_path);
        return true;
    }

    bool CircuitJsonSerializer::loadFromJsonFile(const std::string& file_path, Circuit& circuit)
    {
        SimulationLogger::getInstance().logInfo("Loading circuit from JSON file: " + file_path);

        // 1. Validate file path and format
        if (!CircuitFileValidator::validateFilePath(file_path, false))
        {
            SimulationLogger::getInstance().logError("Invalid file path for loading");
            return false;
        }

        if (!CircuitFileValidator::validateJsonFileFormat(file_path))
        {
            SimulationLogger::getInstance().logError("Invalid JSON file format");
            throw InvalidJsonException("Invalid JSON file format");
            return false;
        }

        // 2. Read file content
        std::ifstream in_file(file_path);
        if (!in_file.is_open())
        {
            SimulationLogger::getInstance().logError("Failed to open file for reading: " + file_path);
            throw FileIOException("Failed to open file for reading: " + file_path);
            return false;
        }

        std::string json_content((std::istreambuf_iterator<char>(in_file)), std::istreambuf_iterator<char>());
        in_file.close();

        // 3. Parse JSON
        json circuit_json;
        try
        {
            circuit_json = json::parse(json_content);
        }
        catch (const json::parse_error& e)
        {
            SimulationLogger::getInstance().logError("JSON parse failed: " + std::string(e.what()));
            throw InvalidJsonException("JSON parse error: " + std::string(e.what()));
            return false;
        }
        catch (const std::exception& e)
        {
            SimulationLogger::getInstance().logError("File read exception: " + std::string(e.what()));
            throw FileIOException("File read exception: " + std::string(e.what()));
            return false;
        }

        // 4. Deserialize JSON to circuit object
        return deserializeFromJson(circuit_json, circuit);
    }

    // ==================== Private Validation Methods ====================

    bool CircuitJsonSerializer::validateJsonRootStructure(const json& json_data)
    {
        std::vector<std::string> required_fields = {"metadata", "components", "nodes", "branches"};
        for (const auto& field : required_fields)
        {
            if (!json_data.contains(field))
            {
                SimulationLogger::getInstance().logError("JSON missing required root field: " + field);
                return false;
            }
        }

        if (!json_data["metadata"].is_object() || !json_data["components"].is_array() ||
            !json_data["nodes"].is_array() || !json_data["branches"].is_array())
        {
            SimulationLogger::getInstance().logError("JSON root field type invalid");
            return false;
        }

        if (!json_data["metadata"].contains("version"))
        {
            SimulationLogger::getInstance().logWarn("JSON missing version field, defaulting to latest");
        }
        else
        {
            std::string version = json_data["metadata"]["version"];
            if (version != "1.0.0")
            {
                SimulationLogger::getInstance().logWarn("Incompatible JSON version: " + version);
            }
        }

        return true;
    }

    bool CircuitJsonSerializer::validateComponentJson(const json& comp_json)
    {
        std::vector<std::string> required_fields = {"id", "type", "name", "x", "y"};
        for (const auto& field : required_fields)
        {
            if (!comp_json.contains(field))
            {
                SimulationLogger::getInstance().logError("Component JSON missing required field: " + field);
                return false;
            }
        }

        if (!comp_json["id"].is_number_integer() || !comp_json["type"].is_string() ||
            !comp_json["name"].is_string() || !comp_json["x"].is_number() || !comp_json["y"].is_number())
        {
            SimulationLogger::getInstance().logError("Component JSON field type invalid");
            return false;
        }

        int id = comp_json["id"];
        if (id < 0)
        {
            SimulationLogger::getInstance().logError("Component ID cannot be negative: " + std::to_string(id));
            return false;
        }

        double x = comp_json["x"];
        double y = comp_json["y"];
        if (x < -10000 || x > 10000 || y < -10000 || y > 10000)
        {
            SimulationLogger::getInstance().logError("Component coordinate out of valid range: (" + std::to_string(x) + ", " + std::to_string(y) + ")");
            return false;
        }

        std::string type = comp_json["type"];
        if (!ComponentFactory::getInstance().isTypeRegistered(type))
        {
            SimulationLogger::getInstance().logError("Unregistered component type: " + type);
            return false;
        }

        return CircuitFileValidator::validateComponentParameters(comp_json);
    }

    bool CircuitJsonSerializer::validateConnectionRelations(const json& json_data)
    {
        std::unordered_set<int> valid_component_ids;
        for (const auto& comp_json : json_data["components"])
        {
            if (comp_json.contains("id") && comp_json["id"].is_number_integer())
            {
                valid_component_ids.insert(static_cast<int>(comp_json["id"]));
            }
        }

        for (const auto& branch_json : json_data["branches"])
        {
            if (!branch_json.contains("start_component_id") || !branch_json.contains("end_component_id"))
            {
                SimulationLogger::getInstance().logError("Branch JSON missing connection component IDs");
                return false;
            }

            int start_id = branch_json["start_component_id"];
            int end_id = branch_json["end_component_id"];

            if (valid_component_ids.find(start_id) == valid_component_ids.end())
            {
                SimulationLogger::getInstance().logError("Branch references invalid start component ID: " + std::to_string(start_id));
                return false;
            }
            if (valid_component_ids.find(end_id) == valid_component_ids.end())
            {
                SimulationLogger::getInstance().logError("Branch references invalid end component ID: " + std::to_string(end_id));
                return false;
            }
        }

        return true;
    }

    // ==================== Serialization Helper Methods ====================

    json CircuitJsonSerializer::serializeComponent(const Component* component)
    {
        json comp_json;
        comp_json["id"] = component->getId();
        comp_json["type"] = component->getTypeName();
        comp_json["name"] = component->getName();
        comp_json["x"] = component->getX();
        comp_json["y"] = component->getY();
        comp_json["rotation"] = component->getRotation();
        comp_json["selected"] = component->isSelected();

        auto properties = component->getAllProperties();
        for (const auto& [key, value] : properties)
        {
            comp_json["properties"][key] = value;
        }

        json ports_array = json::array();
        for (size_t i = 0; i < component->getPorts().size(); ++i)
        {
            const Port* port = component->getPorts()[i];
            json port_json;
            port_json["index"] = i;
            port_json["connected"] = port->isConnected();
            if (port->isConnected() && port->getNode())
            {
                port_json["node_id"] = port->getNode()->getId();
            }
            ports_array.push_back(port_json);
        }
        comp_json["ports"] = ports_array;

        return comp_json;
    }

    json CircuitJsonSerializer::serializeNode(const Node* node)
    {
        json node_json;
        node_json["id"] = node->getId();
        node_json["is_ground"] = node->isGround();
        node_json["voltage"] = node->getVoltage();
        return node_json;
    }

    json CircuitJsonSerializer::serializeBranch(const Branch* branch)
    {
        json branch_json;
        branch_json["id"] = branch->getId();
        branch_json["start_component_id"] = branch->getStartComponent()->getId();
        branch_json["start_port_index"] = branch->getStartPortIndex();
        branch_json["end_component_id"] = branch->getEndComponent()->getId();
        branch_json["end_port_index"] = branch->getEndPortIndex();
        branch_json["resistance"] = branch->getResistance();
        return branch_json;
    }

    // ==================== Deserialization Helper Methods ====================

    Component* CircuitJsonSerializer::deserializeComponent(const json& comp_json)
    {
        int id = comp_json["id"];
        std::string type = comp_json["type"];
        std::string name = comp_json["name"];
        double x = comp_json["x"];
        double y = comp_json["y"];

        Component* component = ComponentFactory::getInstance().create(type, id, x, y);
        if (!component)
        {
            SimulationLogger::getInstance().logError("Failed to create component: " + type);
            return nullptr;
        }

        component->setName(name);
        if (comp_json.contains("rotation"))
        {
            component->setRotation(comp_json["rotation"]);
        }
        if (comp_json.contains("selected"))
        {
            component->setSelected(comp_json["selected"]);
        }

        if (comp_json.contains("properties"))
        {
            for (const auto& [key, value] : comp_json["properties"].items())
            {
                component->setProperty(key, value);
            }
        }

        return component;
    }

    Node* CircuitJsonSerializer::deserializeNode(const json& node_json)
    {
        int id = node_json["id"];
        Node* node = new Node(id);
        if (node_json.contains("is_ground"))
        {
            node->setGround(node_json["is_ground"]);
        }
        if (node_json.contains("voltage"))
        {
            node->setVoltage(node_json["voltage"]);
        }
        return node;
    }

    Branch* CircuitJsonSerializer::deserializeBranch(const json& branch_json, Circuit& circuit)
    {
        int id = branch_json["id"];
        int start_comp_id = branch_json["start_component_id"];
        int start_port_idx = branch_json["start_port_index"];
        int end_comp_id = branch_json["end_component_id"];
        int end_port_idx = branch_json["end_port_index"];

        Component* start_comp = circuit.getComponent(start_comp_id);
        Component* end_comp = circuit.getComponent(end_comp_id);
        if (!start_comp || !end_comp)
        {
            SimulationLogger::getInstance().logError("Branch references non-existent component");
            return nullptr;
        }

        if (start_port_idx < 0 || start_port_idx >= static_cast<int>(start_comp->getPorts().size()) ||
            end_port_idx < 0 || end_port_idx >= static_cast<int>(end_comp->getPorts().size()))
        {
            SimulationLogger::getInstance().logError("Branch references invalid port index");
            return nullptr;
        }

        Branch* branch = new Branch(id, start_comp, start_port_idx, end_comp, end_port_idx);
        if (branch_json.contains("resistance"))
        {
            branch->setResistance(branch_json["resistance"]);
        }

        circuit.connect(start_comp->getPorts()[start_port_idx], end_comp->getPorts()[end_port_idx]);

        return branch;
    }
}