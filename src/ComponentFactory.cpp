#include "ComponentFactory.h"

#include "Ammeter.h"
#include "Bulb.h"
#include "Capacitor.h"
#include "Inductor.h"
#include "PowerSource.h"
#include "Resistor.h"
#include "Switch.h"
#include "Voltmeter.h"

namespace CircuitSim {

ComponentFactory& ComponentFactory::getInstance() {
    static ComponentFactory instance;
    static bool initialized = false;
    if (!initialized) {
        instance.registerBuiltInTypes();
        initialized = true;
    }
    return instance;
}

void ComponentFactory::registerType(const std::string& typeName, Creator creator) {
    creators_[typeName] = std::move(creator);
}

bool ComponentFactory::isTypeRegistered(const std::string& typeName) const {
    return creators_.find(typeName) != creators_.end();
}

Component* ComponentFactory::create(const std::string& typeName, int id, double x, double y) {
    auto it = creators_.find(typeName);
    if (it != creators_.end()) {
        return it->second(id, x, y);
    }
    return nullptr;
}

std::string ComponentFactory::typeToString(ComponentType type) {
    switch (type) {
        case ComponentType::RESISTOR: return "Resistor";
        case ComponentType::POWER_SOURCE: return "PowerSource";
        case ComponentType::BULB: return "Bulb";
        case ComponentType::AMMETER: return "Ammeter";
        case ComponentType::VOLTMETER: return "Voltmeter";
        case ComponentType::SWITCH: return "Switch";
        case ComponentType::CAPACITOR: return "Capacitor";
        case ComponentType::INDUCTOR: return "Inductor";
        default: return "Unknown";
    }
}

ComponentType ComponentFactory::stringToType(const std::string& str) {
    if (str == "Resistor") return ComponentType::RESISTOR;
    if (str == "PowerSource") return ComponentType::POWER_SOURCE;
    if (str == "Bulb") return ComponentType::BULB;
    if (str == "Ammeter") return ComponentType::AMMETER;
    if (str == "Voltmeter") return ComponentType::VOLTMETER;
    if (str == "Switch") return ComponentType::SWITCH;
    if (str == "Capacitor") return ComponentType::CAPACITOR;
    if (str == "Inductor") return ComponentType::INDUCTOR;
    return ComponentType::RESISTOR;
}

void ComponentFactory::registerBuiltInTypes() {
    if (!creators_.empty()) {
        return;
    }

    registerType("Resistor", [](int id, double x, double y) { return new Resistor(id, x, y); });
    registerType("PowerSource", [](int id, double x, double y) { return new PowerSource(id, x, y); });
    registerType("Bulb", [](int id, double x, double y) { return new Bulb(id, x, y); });
    registerType("Ammeter", [](int id, double x, double y) { return new Ammeter(id, x, y); });
    registerType("Voltmeter", [](int id, double x, double y) { return new Voltmeter(id, x, y); });
    registerType("Switch", [](int id, double x, double y) { return new Switch(id, x, y); });
    registerType("Capacitor", [](int id, double x, double y) { return new Capacitor(id, x, y); });
    registerType("Inductor", [](int id, double x, double y) { return new Inductor(id, x, y); });
}

std::vector<std::string> ComponentFactory::getRegisteredTypes() {
    std::vector<std::string> types;
    for (const auto& pair : getInstance().creators_) {
        types.push_back(pair.first);
    }
    return types;
}

} // namespace CircuitSim
