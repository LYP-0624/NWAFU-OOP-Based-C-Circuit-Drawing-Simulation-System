#ifndef CAPACITOR_H
#define CAPACITOR_H

#include "Component.h"

namespace CircuitSim {

// 电容元件
class Capacitor : public Component {
private:
    double capacitance_;  // 电容量(F)
    double charge_;       // 电荷量(C)
    double voltage_;      // 电压(V)
    double current_;      // 电流(A)

public:
    Capacitor(int id, double x, double y, double capacitance = 1e-6);
    ~Capacitor() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    double getCapacitance() const { return capacitance_; }
    void setCapacitance(double c) { capacitance_ = c; }
    double getCharge() const { return charge_; }
    void setCharge(double q) { charge_ = q; }
};

} // namespace CircuitSim

#endif