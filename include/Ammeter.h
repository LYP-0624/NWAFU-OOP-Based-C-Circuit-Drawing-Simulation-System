#ifndef AMMETER_H
#define AMMETER_H

#include "Component.h"

namespace CircuitSim {

class Ammeter : public Component {
private:
    double reading_;

public:
    Ammeter(int id, double x, double y);
    ~Ammeter() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    double getReading() const { return reading_; }
};

} // namespace CircuitSim

#endif