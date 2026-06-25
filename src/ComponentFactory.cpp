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

// 获取单例实例（首次调用时注册内置类型）
ComponentFactory& ComponentFactory::getInstance() {
    static ComponentFactory instance;
    static bool initialized = false;
    if (!initialized) {
        instance.registerBuiltInTypes();
        initialized = true;
    }
    return instance;
}

// 注册新的元器件类型和对应的创建函数
void ComponentFactory::registerType(const std::string& typeName, Creator creator) {
    creators_[typeName] = std::move(creator);
}

// 检查指定类型是否已注册
bool ComponentFactory::isTypeRegistered(const std::string& typeName) const {
    return creators_.find(typeName) != creators_.end();
}

// 根据类型名称创建对应的元器件实例
Component* ComponentFactory::create(const std::string& typeName, int id, double x, double y) {
    auto it = creators_.find(typeName);
    if (it != creators_.end()) {
        return it->second(id, x, y);
    }
    return nullptr;
}

// 将枚举类型转换为字符串名称
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

// 将字符串名称转换为枚举类型（默认返回电阻）
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

// 注册所有内置元器件类型的创建函数
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

// 获取所有已注册的类型名称列表
std::vector<std::string> ComponentFactory::getRegisteredTypes() {
    std::vector<std::string> types;
    for (const auto& pair : getInstance().creators_) {
        types.push_back(pair.first);
    }
    return types;
}

} // namespace CircuitSim