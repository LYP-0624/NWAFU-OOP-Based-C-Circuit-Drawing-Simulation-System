#include "Voltmeter.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

Voltmeter::Voltmeter(int id, double x, double y)
    : Component(id, "Voltmeter", x, y), reading_(0.0) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

Voltmeter::~Voltmeter() = default;

ComponentType Voltmeter::getType() const {
    return ComponentType::VOLTMETER;
}

void Voltmeter::simulate(double voltage, double current) {
    reading_ = voltage;
    properties_["resistance"] = 1e9;
    properties_["voltage"] = voltage;
    properties_["current"] = current;
    properties_["reading"] = reading_;
    properties_["power"] = voltage * current;
    properties_["isActive"] = 1.0;
}

double Voltmeter::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

std::map<std::string, double> Voltmeter::getAllProperties() const {
    return properties_;
}

Component* Voltmeter::clone() const {
    return new Voltmeter(id_, x_, y_);
}

REGISTER_COMPONENT(Voltmeter, Voltmeter)

} // namespace CircuitSim
