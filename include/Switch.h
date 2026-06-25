#ifndef SWITCH_H
#define SWITCH_H

#include "Component.h"

namespace CircuitSim {

// 开关元件
class Switch : public Component {
private:
    bool isOn_;          // 开关状态(闭合/断开)
    double resistanceOn_;  // 闭合电阻(Ω)
    double resistanceOff_; // 断开电阻(Ω)

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
