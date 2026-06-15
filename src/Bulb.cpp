#include "Bulb.h"
#include "ComponentFactory.h"
#include "Port.h"


namespace CircuitSim {

Bulb::Bulb(int id, double x, double y, double resistance, double ratedPower)
    : Component(id, "Bulb", x, y), resistance_(resistance), ratedPower_(ratedPower),
      current_(0.0), voltage_(0.0), isLit_(false) {
    addPort(new Port(this, 0));
    addPort(new Port(this, 1));
}

Bulb::~Bulb() = default;

ComponentType Bulb::getType() const {
    return ComponentType::BULB;
}

void Bulb::simulate(double voltage, double current) {
    voltage_ = voltage;
    current_ = current;
    double power = voltage_ * current_;
    isLit_ = (power > ratedPower_ * 0.1); // 超过10%额定功率即发光
    
    properties_["resistance"] = resistance_;
    properties_["voltage"] = voltage_;
    properties_["current"] = current_;
    properties_["power"] = power;
    properties_["isActive"] = isLit_ ? 1.0 : 0.0;
    properties_["ratedPower"] = ratedPower_;
}

double Bulb::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

std::map<std::string, double> Bulb::getAllProperties() const {
    return properties_;
}

Component* Bulb::clone() const {
    return new Bulb(id_, x_, y_, resistance_, ratedPower_);
}

REGISTER_COMPONENT(Bulb, Bulb)

} // namespace CircuitSim