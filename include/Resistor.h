#ifndef RESISTOR_H
#define RESISTOR_H

#include "Component.h"

namespace CircuitSim {

class Resistor : public Component {
private:
    double resistance_;
    double current_;
    double voltage_;

public:
    Resistor(int id, double x, double y, double resistance = 100.0);
    ~Resistor() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    void setResistance(double r) { resistance_ = r; }
    double getResistance() const { return resistance_; }
};

} // namespace CircuitSim

#endif