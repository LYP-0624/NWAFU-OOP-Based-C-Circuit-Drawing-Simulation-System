#include "CircuitFileValidator.h"

#include <algorithm>
#include <cctype>
#include <fstream>

namespace CircuitSim {

namespace {

bool endsWithJson(const std::string& path) {
    if (path.size() < 5) {
        return false;
    }
    std::string suffix = path.substr(path.size() - 5);
    std::transform(suffix.begin(), suffix.end(), suffix.begin(), [](unsigned char c) {
        return static_cast<char>(std::tolower(c));
    });
    return suffix == ".json";
}

} // namespace

bool CircuitFileValidator::validateFilePath(const std::string& file_path, bool for_writing) {
    if (file_path.empty() || !endsWithJson(file_path)) {
        return false;
    }

    if (!for_writing) {
        std::ifstream in(file_path, std::ios::binary);
        return in.good();
    }

    return true;
}

bool CircuitFileValidator::validateJsonFileFormat(const std::string& file_path) {
    std::ifstream in(file_path, std::ios::binary);
    if (!in.is_open()) {
        return false;
    }

    char first = '\0';
    in >> std::ws;
    if (!in.get(first)) {
        return false;
    }
    return first == '{' || first == '[';
}

bool CircuitFileValidator::validateComponentParameters(const json& comp_json) {
    if (!comp_json.contains("type") || !comp_json["type"].is_string()) {
        return false;
    }

    const std::string type = comp_json["type"].get<std::string>();
    const auto props = comp_json.contains("properties") && comp_json["properties"].is_object()
        ? comp_json["properties"]
        : json::object();

    if (type == "Resistor" || type == "Bulb") {
        return props.contains("resistance") && props["resistance"].is_number() && props["resistance"].get<double>() > 0.0;
    }
    if (type == "PowerSource") {
        return props.contains("value") && props["value"].is_number() && props["value"].get<double>() != 0.0;
    }
    if (type == "Switch") {
        return props.contains("resistance_on") && props.contains("resistance_off");
    }
    if (type == "Capacitor") {
        return props.contains("capacitance") && props["capacitance"].is_number() && props["capacitance"].get<double>() > 0.0;
    }
    if (type == "Inductor") {
        return props.contains("inductance") && props["inductance"].is_number() && props["inductance"].get<double>() > 0.0;
    }

    return true;
}

} // namespace CircuitSim
