#ifndef VOLTMETER_H
#define VOLTMETER_H

#include "Component.h"

namespace CircuitSim {

// 电压表元件
class Voltmeter : public Component {
private:
    double reading_;  // 读数(V)

public:
    Voltmeter(int id, double x, double y);
    ~Voltmeter() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    double getReading() const { return reading_; }
};

} // namespace CircuitSim

#endif