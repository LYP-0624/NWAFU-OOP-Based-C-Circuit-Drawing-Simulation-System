#include "PowerSource.h"
#include "ComponentFactory.h"

namespace CircuitSim {

PowerSource::PowerSource(int id, double x, double y, double voltage)
    : Component(id, "PowerSource", x, y), voltage_(voltage) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

PowerSource::~PowerSource() = default;

ComponentType PowerSource::getType() const {
    return ComponentType::POWER_SOURCE;
}

void PowerSource::simulate(double voltage, double current) {
    properties_["voltage"] = voltage_;
    properties_["current"] = current;
    properties_["power"] = voltage_ * current;
    properties_["isActive"] = 1.0;
}

double PowerSource::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

std::map<std::string, double> PowerSource::getAllProperties() const {
    return properties_;
}

Component* PowerSource::clone() const {
    return new PowerSource(id_, x_, y_, voltage_);
}

REGISTER_COMPONENT(PowerSource, PowerSource)

} // namespace CircuitSim