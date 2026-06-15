#include "Resistor.h"
#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

Resistor::Resistor(int id, double x, double y, double resistance)
    : Component(id, "Resistor", x, y), resistance_(resistance), current_(0.0), voltage_(0.0) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

Resistor::~Resistor() = default;

ComponentType Resistor::getType() const {
    return ComponentType::RESISTOR;
}

void Resistor::simulate(double voltage, double current) {
    voltage_ = voltage;
    current_ = current;
    properties_["resistance"] = resistance_;
    properties_["voltage"] = voltage_;
    properties_["current"] = current_;
    properties_["power"] = voltage_ * current_;
    properties_["isActive"] = (current_ > 1e-10) ? 1.0 : 0.0;
}

double Resistor::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

std::map<std::string, double> Resistor::getAllProperties() const {
    return properties_;
}

Component* Resistor::clone() const {
    return new Resistor(id_, x_, y_, resistance_);
}

REGISTER_COMPONENT(Resistor, Resistor)

} // namespace CircuitSim