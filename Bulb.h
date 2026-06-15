#ifndef BULB_H
#define BULB_H

#include "Component.h"

namespace CircuitSim {

class Bulb : public Component {
private:
    double resistance_;
    double ratedPower_;
    double current_;
    double voltage_;
    bool isLit_;

public:
    Bulb(int id, double x, double y, double resistance = 10.0, double ratedPower = 0.5);
    ~Bulb() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    bool isLit() const { return isLit_; }
};

} // namespace CircuitSim

#endif