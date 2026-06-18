#include "CircuitJsonSerializer.h"

#include <fstream>
#include <iterator>
#include <unordered_map>

#include "Ammeter.h"
#include "Bulb.h"
#include "Capacitor.h"
#include "CircuitFileValidator.h"
#include "ComponentFactory.h"
#include "Inductor.h"
#include "Port.h"
#include "PowerSource.h"
#include "Resistor.h"
#include "Switch.h"
#include "Voltmeter.h"

namespace CircuitSim {

namespace {

SourceType sourceTypeFromJson(const json& props) {
    if (!props.contains("sourceType")) {
        return SourceType::VOLTAGE_SOURCE;
    }
    return props["sourceType"].get<double>() < 0.5 ? SourceType::VOLTAGE_SOURCE
                                                    : SourceType::CURRENT_SOURCE;
}

} // namespace

json CircuitJsonSerializer::serializeToJson(const Circuit& circuit) {
    json root;
    root["metadata"]["version"] = "1.0.0";
    root["metadata"]["circuit_name"] = circuit.getName();
    root["metadata"]["next_component_id"] = circuit.getNextComponentId();
    root["metadata"]["next_node_id"] = circuit.getNextNodeId();
    root["metadata"]["next_branch_id"] = circuit.getNextBranchId();
    root["metadata"]["component_count"] = circuit.getComponentCount();
    root["metadata"]["node_count"] = circuit.getNodeCount();
    root["metadata"]["branch_count"] = circuit.getBranchCount();
    root["metadata"]["is_solved"] = circuit.isSolved();

    json components = json::array();
    for (const auto& [id, comp] : circuit.getComponents()) {
        components.push_back(serializeComponent(comp));
    }
    root["components"] = std::move(components);

    json nodes = json::array();
    for (const auto* node : circuit.getNodes()) {
        nodes.push_back(serializeNode(node));
    }
    root["nodes"] = std::move(nodes);

    json branches = json::array();
    for (const auto* branch : circuit.getBranches()) {
        branches.push_back(serializeBranch(branch));
    }
    root["branches"] = std::move(branches);

    json results = json::array();
    for (const auto& result : circuit.getSimulationResults()) {
        json r;
        r["component_id"] = result.componentId;
        r["component_name"] = result.componentName;
        r["component_type"] = result.componentType;
        r["voltage"] = result.voltage;
        r["current"] = result.current;
        r["power"] = result.power;
        r["is_active"] = result.isActive;
        r["extra"] = result.extra;
        results.push_back(std::move(r));
    }
    root["simulation_results"] = std::move(results);
    return root;
}

bool CircuitJsonSerializer::deserializeFromJson(const json& json_data, Circuit& circuit) {
    if (!validateJsonRootStructure(json_data)) {
        return false;
    }

    circuit.clear();
    circuit.setName(json_data["metadata"].value("circuit_name", std::string{}));
    circuit.setIdCounters(json_data["metadata"].value("next_component_id", 1),
                          json_data["metadata"].value("next_node_id", 1),
                          json_data["metadata"].value("next_branch_id", 1));

    std::unordered_map<int, Node*> nodeMap;
    if (json_data.contains("nodes")) {
        for (const auto& node_json : json_data["nodes"]) {
            Node* node = deserializeNode(node_json);
            if (!node) {
                continue;
            }
            circuit.addNode(node);
            nodeMap[node->getId()] = node;
        }
    }

    if (json_data.contains("components")) {
        for (const auto& comp_json : json_data["components"]) {
            if (!validateComponentJson(comp_json)) {
                return false;
            }

            Component* component = deserializeComponent(comp_json);
            if (!component) {
                return false;
            }
            circuit.addComponent(component);

            const auto ports = component->getPorts();
            if (comp_json.contains("ports") && comp_json["ports"].is_array()) {
                for (const auto& port_json : comp_json["ports"]) {
                    const int idx = port_json.value("index", -1);
                    if (idx < 0 || idx >= static_cast<int>(ports.size()) || !ports[idx]) {
                        continue;
                    }

                    const int nodeId = port_json.value("node_id", -1);
                    if (nodeId < 0) {
                        continue;
                    }

                    Node* node = nullptr;
                    auto it = nodeMap.find(nodeId);
                    if (it != nodeMap.end()) {
                        node = it->second;
                    } else {
                        node = new Node(nodeId);
                        circuit.addNode(node);
                        nodeMap[nodeId] = node;
                    }
                    ports[idx]->connectTo(node);
                }
            }
        }
    }

    if (json_data.contains("branches")) {
        for (const auto& branch_json : json_data["branches"]) {
            Branch* branch = deserializeBranch(branch_json, circuit);
            if (branch) {
                circuit.addBranch(branch);
            }
        }
    }

    circuit.setIdCounters(json_data["metadata"].value("next_component_id", circuit.getNextComponentId()),
                          json_data["metadata"].value("next_node_id", circuit.getNextNodeId()),
                          json_data["metadata"].value("next_branch_id", circuit.getNextBranchId()));

    (void)json_data;
    return true;
}

bool CircuitJsonSerializer::saveToJsonFile(const Circuit& circuit, const std::string& file_path) {
    if (!CircuitFileValidator::validateFilePath(file_path, true)) {
        return false;
    }

    const json circuit_json = serializeToJson(circuit);
    std::ofstream out(file_path, std::ios::binary);
    if (!out.is_open()) {
        return false;
    }

    out << circuit_json.dump(4);
    return true;
}

bool CircuitJsonSerializer::loadFromJsonFile(const std::string& file_path, Circuit& circuit) {
    if (!CircuitFileValidator::validateFilePath(file_path, false) ||
        !CircuitFileValidator::validateJsonFileFormat(file_path)) {
        return false;
    }

    std::ifstream in(file_path, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }

    json circuit_json;
    try {
        in >> circuit_json;
    } catch (...) {
        return false;
    }

    return deserializeFromJson(circuit_json, circuit);
}

bool CircuitJsonSerializer::validateJsonRootStructure(const json& json_data) {
    return json_data.contains("metadata") &&
           json_data.contains("components") &&
           json_data.contains("nodes") &&
           json_data.contains("branches");
}

bool CircuitJsonSerializer::validateComponentJson(const json& comp_json) {
    if (!comp_json.contains("id") || !comp_json.contains("type") || !comp_json.contains("x") ||
        !comp_json.contains("y")) {
        return false;
    }
    return CircuitFileValidator::validateComponentParameters(comp_json);
}

json CircuitJsonSerializer::serializeComponent(const Component* component) {
    json comp;
    comp["id"] = component->getId();
    comp["type"] = component->getTypeName();
    comp["name"] = component->getName();
    comp["x"] = component->getX();
    comp["y"] = component->getY();
    comp["rotation"] = component->getRotation();
    comp["selected"] = component->isSelected();
    comp["properties"] = component->getAllProperties();

    switch (component->getType()) {
        case ComponentType::RESISTOR: {
            const auto* r = static_cast<const Resistor*>(component);
            comp["properties"]["resistance"] = r->getResistance();
            break;
        }
        case ComponentType::POWER_SOURCE: {
            const auto* s = static_cast<const PowerSource*>(component);
            comp["properties"]["value"] = s->getValue();
            comp["properties"]["sourceType"] = s->isVoltageSource() ? 0.0 : 1.0;
            break;
        }
        case ComponentType::BULB: {
            const auto* b = static_cast<const Bulb*>(component);
            comp["properties"]["resistance"] = b->getResistance();
            comp["properties"]["ratedPower"] = b->getRatedPower();
            break;
        }
        case ComponentType::SWITCH: {
            const auto* sw = static_cast<const Switch*>(component);
            comp["properties"]["state"] = sw->isOn() ? 1.0 : 0.0;
            comp["properties"]["resistance_on"] = sw->getResistanceOn();
            comp["properties"]["resistance_off"] = sw->getResistanceOff();
            break;
        }
        case ComponentType::CAPACITOR: {
            const auto* c = static_cast<const Capacitor*>(component);
            comp["properties"]["capacitance"] = c->getCapacitance();
            break;
        }
        case ComponentType::INDUCTOR: {
            const auto* l = static_cast<const Inductor*>(component);
            comp["properties"]["inductance"] = l->getInductance();
            break;
        }
        default:
            break;
    }

    json ports = json::array();
    const auto portList = component->getPorts();
    for (size_t i = 0; i < portList.size(); ++i) {
        json p;
        p["index"] = static_cast<int>(i);
        p["connected"] = portList[i] && portList[i]->isConnected();
        p["node_id"] = (portList[i] && portList[i]->isConnected() && portList[i]->getNode())
            ? portList[i]->getNode()->getId()
            : -1;
        ports.push_back(std::move(p));
    }
    comp["ports"] = std::move(ports);
    return comp;
}

json CircuitJsonSerializer::serializeNode(const Node* node) {
    json nodeJson;
    nodeJson["id"] = node->getId();
    nodeJson["is_ground"] = node->isGround();
    nodeJson["voltage"] = node->getVoltage();

    json ports = json::array();
    for (const auto* port : node->getPorts()) {
        json p;
        p["component_id"] = port && port->getParent() ? port->getParent()->getId() : -1;
        p["port_index"] = port ? port->getIndex() : -1;
        ports.push_back(std::move(p));
    }
    nodeJson["ports"] = std::move(ports);
    return nodeJson;
}

json CircuitJsonSerializer::serializeBranch(const Branch* branch) {
    json branchJson;
    branchJson["id"] = branch->getId();
    branchJson["start_component_id"] = branch->getStartComponent() ? branch->getStartComponent()->getId() : -1;
    branchJson["start_port_index"] = branch->getStartPortIndex();
    branchJson["end_component_id"] = branch->getEndComponent() ? branch->getEndComponent()->getId() : -1;
    branchJson["end_port_index"] = branch->getEndPortIndex();
    branchJson["node_a_id"] = branch->getNodeA() ? branch->getNodeA()->getId() : -1;
    branchJson["node_b_id"] = branch->getNodeB() ? branch->getNodeB()->getId() : -1;
    branchJson["resistance"] = branch->getResistance();
    return branchJson;
}

Component* CircuitJsonSerializer::deserializeComponent(const json& comp_json) {
    Component* component = ComponentFactory::getInstance().create(
        comp_json.value("type", std::string{}),
        comp_json.value("id", 0),
        comp_json.value("x", 0.0),
        comp_json.value("y", 0.0));
    if (!component) {
        return nullptr;
    }

    component->setName(comp_json.value("name", component->getName()));
    component->setRotation(comp_json.value("rotation", 0.0));
    component->setSelected(comp_json.value("selected", false));

    const auto props = comp_json.contains("properties") && comp_json["properties"].is_object()
        ? comp_json["properties"]
        : json::object();
    for (const auto& [key, value] : props.items()) {
        if (value.is_number()) {
            component->setProperty(key, value.get<double>());
        }
    }

    if (auto* r = dynamic_cast<Resistor*>(component); r && props.contains("resistance")) {
        r->setResistance(props["resistance"].get<double>());
    } else if (auto* b = dynamic_cast<Bulb*>(component); b && props.contains("resistance")) {
        b->setResistance(props["resistance"].get<double>());
        if (props.contains("ratedPower")) {
            b->setRatedPower(props["ratedPower"].get<double>());
        }
    } else if (auto* s = dynamic_cast<PowerSource*>(component); s) {
        if (props.contains("value")) {
            s->setValue(props["value"].get<double>());
        }
        s->setSourceType(sourceTypeFromJson(props));
    } else if (auto* sw = dynamic_cast<Switch*>(component); sw) {
        if (props.contains("state")) {
            sw->setState(props["state"].get<double>() > 0.5);
        }
    } else if (auto* c = dynamic_cast<Capacitor*>(component); c && props.contains("capacitance")) {
        c->setCapacitance(props["capacitance"].get<double>());
        if (props.contains("charge")) {
            c->setCharge(props["charge"].get<double>());
        }
    } else if (auto* l = dynamic_cast<Inductor*>(component); l && props.contains("inductance")) {
        l->setInductance(props["inductance"].get<double>());
    }

    return component;
}

Node* CircuitJsonSerializer::deserializeNode(const json& node_json) {
    if (!node_json.contains("id")) {
        return nullptr;
    }

    Node* node = new Node(node_json["id"].get<int>());
    node->setGround(node_json.value("is_ground", false));
    node->setVoltage(node_json.value("voltage", 0.0));
    return node;
}

Branch* CircuitJsonSerializer::deserializeBranch(const json& branch_json, Circuit& circuit) {
    const int startId = branch_json.value("start_component_id", -1);
    const int startPort = branch_json.value("start_port_index", -1);
    const int endId = branch_json.value("end_component_id", -1);
    const int endPort = branch_json.value("end_port_index", -1);
    Component* startComp = circuit.getComponent(startId);
    Component* endComp = circuit.getComponent(endId);
    if (!startComp || !endComp || startPort < 0 || endPort < 0) {
        return nullptr;
    }

    Branch* branch = new Branch(branch_json.value("id", 0), startComp, startPort, endComp, endPort);
    branch->setResistance(branch_json.value("resistance", 0.0));
    branch->setNodeA(startComp->getPort(static_cast<size_t>(startPort)) ?
                     startComp->getPort(static_cast<size_t>(startPort))->getNode() : nullptr);
    branch->setNodeB(endComp->getPort(static_cast<size_t>(endPort)) ?
                     endComp->getPort(static_cast<size_t>(endPort))->getNode() : nullptr);
    return branch;
}

} // namespace CircuitSim
