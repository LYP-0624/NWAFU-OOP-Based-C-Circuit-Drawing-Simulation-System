#include "Ground.h"
#include "Port.h"
#include "ComponentFactory.h"

namespace CircuitSim {

Ground::Ground(int id, double x, double y)
    : Component(id, "Ground", x, y), voltage_(0.0) {
    addPort(new Port(this, 0));
}

Ground::~Ground() = default;

ComponentType Ground::getType() const {
    return ComponentType::GROUND;
}

void Ground::simulate(double voltage, double current) {
    voltage_ = 0.0;
    properties_["voltage"] = 0.0;
    properties_["current"] = current;
    properties_["power"] = 0.0;
    properties_["isActive"] = 1.0;
}

double Ground::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    if (key == "voltage") return 0.0;
    return 0.0;
}

std::map<std::string, double> Ground::getAllProperties() const {
    return properties_;
}

Component* Ground::clone() const {
    return new Ground(id_, x_, y_);
}

REGISTER_COMPONENT(Ground, Ground)

} // namespace CircuitSim