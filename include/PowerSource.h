#ifndef POWER_SOURCE_H
#define POWER_SOURCE_H

#include "Component.h"

namespace CircuitSim {

enum class SourceType {
    VOLTAGE_SOURCE,
    CURRENT_SOURCE
};

class PowerSource : public Component {
private:
    double value_;
    SourceType sourceType_;

public:
    PowerSource(int id, double x, double y, double value = 5.0, SourceType type = SourceType::VOLTAGE_SOURCE);
    ~PowerSource() override;

    ComponentType getType() const override;
    void simulate(double voltage, double current) override;
    double getProperty(const std::string& key) const override;
    std::map<std::string, double> getAllProperties() const override;
    Component* clone() const override;

    void setValue(double v) { value_ = v; }
    double getValue() const { return value_; }
    void setSourceType(SourceType type) { sourceType_ = type; }
    SourceType getSourceType() const { return sourceType_; }
    bool isVoltageSource() const { return sourceType_ == SourceType::VOLTAGE_SOURCE; }
    bool isCurrentSource() const { return sourceType_ == SourceType::CURRENT_SOURCE; }
};

} // namespace CircuitSim

#endif