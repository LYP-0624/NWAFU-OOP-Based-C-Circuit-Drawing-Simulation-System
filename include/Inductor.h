#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "Component.h"

namespace CircuitSim {

// 电感元件
class Inductor : public Component {
private:
    double inductance_;  // 电感量(H)
    double current_;     // 电流(A)
    double voltage_;     // 电压(V)

public:
    Inductor(int id, double x, double y, double inductance = 1e-3);
    ~Inductor() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    double getInductance() const { return inductance_; }
    void setInductance(double l) { inductance_ = l; }
};

} // namespace CircuitSim

#endif