#include "ComponentFactory.h"
#include "Resistor.h"
#include "PowerSource.h"
#include "Bulb.h"
#include "Ammeter.h"
#include "Voltmeter.h"
#include "Switch.h"
#include "Ground.h"
#include "Capacitor.h"
#include "Inductor.h"

namespace CircuitSim {

ComponentFactory& ComponentFactory::getInstance() {
    static ComponentFactory instance;
    return instance;
}

void ComponentFactory::registerType(const std::string& typeName, Creator creator) {
    creators_[typeName] = creator;
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
        case ComponentType::GROUND: return "Ground";
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
    if (str == "Ground") return ComponentType::GROUND;
    if (str == "Capacitor") return ComponentType::CAPACITOR;
    if (str == "Inductor") return ComponentType::INDUCTOR;
    return ComponentType::RESISTOR;
}

void ComponentFactory::registerBuiltInTypes() {
    // 通过宏自动注册，无需手动调用
}

std::vector<std::string> ComponentFactory::getRegisteredTypes() {
    std::vector<std::string> types;
    for (const auto& pair : getInstance().creators_) {
        types.push_back(pair.first);
    }
    return types;
}

} // namespace CircuitSim