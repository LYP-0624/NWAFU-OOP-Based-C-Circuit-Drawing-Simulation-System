#ifndef POWER_SOURCE_H
#define POWER_SOURCE_H

#include "Component.h"

namespace CircuitSim {

class PowerSource : public Component {
private:
    double voltage_;

public:
    PowerSource(int id, double x, double y, double voltage = 5.0);
    ~PowerSource() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    void setVoltage(double v) { voltage_ = v; }
    double getVoltage() const { return voltage_; }
};

} // namespace CircuitSim

#endif