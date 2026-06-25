#ifndef BULB_H
#define BULB_H

#include "Component.h"

namespace CircuitSim {

// 灯泡元件
class Bulb : public Component {
private:
    double resistance_;  // 电阻值(Ω)
    double ratedPower_;  // 额定功率(W)
    double current_;     // 电流值(A)
    double voltage_;     // 电压值(V)
    bool isLit_;         // 是否点亮

public:
    Bulb(int id, double x, double y, double resistance = 10.0, double ratedPower = 0.5);
    ~Bulb() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    bool isLit() const { return isLit_; }
    double getResistance() const { return resistance_; }
    void setResistance(double r) { resistance_ = r; }
    double getRatedPower() const { return ratedPower_; }
    void setRatedPower(double p) { ratedPower_ = p; }
};

} // namespace CircuitSim

#endif
