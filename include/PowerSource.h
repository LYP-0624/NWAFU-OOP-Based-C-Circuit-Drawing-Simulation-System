#ifndef POWER_SOURCE_H
#define POWER_SOURCE_H

#include "Component.h"

namespace CircuitSim {

// 电源类型
enum class SourceType {
    VOLTAGE_SOURCE,  // 电压源
    CURRENT_SOURCE   // 电流源
};

// 电源元件
class PowerSource : public Component {
private:
    double value_;       // 源值(电压/V 或电流/A)
    SourceType sourceType_; // 源类型

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