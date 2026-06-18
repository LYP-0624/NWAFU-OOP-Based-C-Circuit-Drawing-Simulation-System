#ifndef SWITCH_H
#define SWITCH_H

#include "Component.h"

namespace CircuitSim {

class Switch : public Component {
private:
    bool isOn_;
    double resistanceOn_;
    double resistanceOff_;

public:
    Switch(int id, double x, double y, bool isOn = false);
    ~Switch() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    void toggle() { isOn_ = !isOn_; }
    void setState(bool on) { isOn_ = on; }
    bool isOn() const { return isOn_; }
    double getResistanceOn() const { return resistanceOn_; }
    double getResistanceOff() const { return resistanceOff_; }
};

} // namespace CircuitSim

#endif
