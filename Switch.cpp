#include "Switch.h"
#include "ComponentFactory.h"

namespace CircuitSim {

Switch::Switch(int id, double x, double y, bool isOn)
    : Component(id, "Switch", x, y), isOn_(isOn),
      resistanceOn_(0.001), resistanceOff_(1e9) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

Switch::~Switch() = default;

ComponentType Switch::getType() const {
    return ComponentType::SWITCH;
}

void Switch::simulate(double voltage, double current) {
    double r = isOn_ ? resistanceOn_ : resistanceOff_;
    properties_["resistance"] = r;
    properties_["voltage"] = voltage;
    properties_["current"] = current;
    properties_["power"] = voltage * current;
    properties_["isActive"] = isOn_ ? 1.0 : 0.0;
    properties_["state"] = isOn_ ? 1.0 : 0.0;
}

double Switch::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

std::map<std::string, double> Switch::getAllProperties() const {
    return properties_;
}

Component* Switch::clone() const {
    return new Switch(id_, x_, y_, isOn_);
}

REGISTER_COMPONENT(Switch, Switch)

} // namespace CircuitSim