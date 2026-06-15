#ifndef GROUND_H
#define GROUND_H

#include "Component.h"

namespace CircuitSim {

class Ground : public Component {
private:
    double voltage_;

public:
    Ground(int id, double x, double y);
    ~Ground() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    double getVoltage() const { return voltage_; }
};

} // namespace CircuitSim

#endif