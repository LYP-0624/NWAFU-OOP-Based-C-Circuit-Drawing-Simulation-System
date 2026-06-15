#ifndef INDUCTOR_H
#define INDUCTOR_H

#include "Component.h"

namespace CircuitSim {

class Inductor : public Component {
private:
    double inductance_;
    double current_;
    double voltage_;

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