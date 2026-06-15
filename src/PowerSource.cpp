#include "PowerSource.h"
#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

PowerSource::PowerSource(int id, double x, double y, double value, SourceType type)
    : Component(id, "PowerSource", x, y), value_(value), sourceType_(type) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

PowerSource::~PowerSource() = default;

ComponentType PowerSource::getType() const {
    return ComponentType::POWER_SOURCE;
}

void PowerSource::simulate(double voltage, double current) {
    properties_["voltage"] = isVoltageSource() ? value_ : voltage;
    properties_["current"] = isCurrentSource() ? value_ : current;
    properties_["power"] = properties_["voltage"] * properties_["current"];
    properties_["isActive"] = 1.0;
    properties_["sourceType"] = isVoltageSource() ? 0.0 : 1.0;
    properties_["value"] = value_;
}

double PowerSource::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    if (key == "voltage") {
        return isVoltageSource() ? value_ : 0.0;
    }
    if (key == "current") {
        return isCurrentSource() ? value_ : 0.0;
    }
    if (key == "value") {
        return value_;
    }
    if (key == "sourceType") {
        return isVoltageSource() ? 0.0 : 1.0;
    }
    return 0.0;
}

std::map<std::string, double> PowerSource::getAllProperties() const {
    return properties_;
}

Component* PowerSource::clone() const {
    return new PowerSource(id_, x_, y_, value_, sourceType_);
}

REGISTER_COMPONENT(PowerSource, PowerSource)

} // namespace CircuitSim