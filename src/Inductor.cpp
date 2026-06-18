#include "Inductor.h"

#include <cmath>

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

Inductor::Inductor(int id, double x, double y, double inductance)
    : Component(id, "Inductor", x, y), inductance_(inductance),
      current_(0.0), voltage_(0.0) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

Inductor::~Inductor() = default;

ComponentType Inductor::getType() const {
    return ComponentType::INDUCTOR;
}

void Inductor::simulate(double voltage, double current) {
    voltage_ = voltage;
    current_ = current;

    properties_["inductance"] = inductance_;
    properties_["voltage"] = voltage_;
    properties_["current"] = current_;
    properties_["power"] = voltage_ * current_;
    properties_["isActive"] = (std::abs(current_) > 1e-10) ? 1.0 : 0.0;
}

double Inductor::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    if (key == "inductance") {
        return inductance_;
    }
    return 0.0;
}

std::map<std::string, double> Inductor::getAllProperties() const {
    return properties_;
}

Component* Inductor::clone() const {
    return new Inductor(id_, x_, y_, inductance_);
}

REGISTER_COMPONENT(Inductor, Inductor)

} // namespace CircuitSim
