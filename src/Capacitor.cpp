#include "Capacitor.h"

#include <cmath>

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

Capacitor::Capacitor(int id, double x, double y, double capacitance)
    : Component(id, "Capacitor", x, y), capacitance_(capacitance),
      charge_(0.0), voltage_(0.0), current_(0.0) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

Capacitor::~Capacitor() = default;

ComponentType Capacitor::getType() const {
    return ComponentType::CAPACITOR;
}

void Capacitor::simulate(double voltage, double current) {
    voltage_ = voltage;
    current_ = current;
    charge_ = capacitance_ * voltage_;

    properties_["capacitance"] = capacitance_;
    properties_["voltage"] = voltage_;
    properties_["current"] = current_;
    properties_["charge"] = charge_;
    properties_["power"] = voltage_ * current_;
    properties_["isActive"] = (std::abs(current_) > 1e-10) ? 1.0 : 0.0;
}

double Capacitor::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    if (key == "capacitance") {
        return capacitance_;
    }
    if (key == "charge") {
        return charge_;
    }
    return 0.0;
}

std::map<std::string, double> Capacitor::getAllProperties() const {
    return properties_;
}

Component* Capacitor::clone() const {
    return new Capacitor(id_, x_, y_, capacitance_);
}

REGISTER_COMPONENT(Capacitor, Capacitor)

} // namespace CircuitSim
