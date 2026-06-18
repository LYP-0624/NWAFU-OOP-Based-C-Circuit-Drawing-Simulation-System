#include "Ammeter.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

Ammeter::Ammeter(int id, double x, double y)
    : Component(id, "Ammeter", x, y), reading_(0.0) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

Ammeter::~Ammeter() = default;

ComponentType Ammeter::getType() const {
    return ComponentType::AMMETER;
}

void Ammeter::simulate(double voltage, double current) {
    reading_ = current;
    properties_["resistance"] = 0.001;
    properties_["voltage"] = voltage;
    properties_["current"] = current;
    properties_["reading"] = reading_;
    properties_["power"] = voltage * current;
    properties_["isActive"] = 1.0;
}

double Ammeter::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

std::map<std::string, double> Ammeter::getAllProperties() const {
    return properties_;
}

Component* Ammeter::clone() const {
    return new Ammeter(id_, x_, y_);
}

REGISTER_COMPONENT(Ammeter, Ammeter)

} // namespace CircuitSim
