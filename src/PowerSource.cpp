#include "PowerSource.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化电源，设置电压/电流值和类型，添加两个端口
PowerSource::PowerSource(int id, double x, double y, double value, SourceType type)
    : Component(id, "PowerSource", x, y), value_(value), sourceType_(type) {
    addPort(new Port(this, 0));  // 正极端口
    addPort(new Port(this, 1));  // 负极端口
}

// 析构函数：使用默认实现
PowerSource::~PowerSource() = default;

// 返回元器件类型为电源
ComponentType PowerSource::getType() const {
    return ComponentType::POWER_SOURCE;
}

// 仿真计算：根据电源类型设置电压或电流值
void PowerSource::simulate(double voltage, double current) {
    properties_["value"] = value_;                          // 电源设定值
    properties_["sourceType"] = isVoltageSource() ? 0.0 : 1.0;  // 0=电压源, 1=电流源
    properties_["voltage"] = isVoltageSource() ? value_ : voltage;  // 电压源输出设定电压，否则为外部电压
    properties_["current"] = isCurrentSource() ? value_ : current;  // 电流源输出设定电流，否则为外部电流
    properties_["power"] = properties_["voltage"] * properties_["current"];  // 功率
    properties_["isActive"] = 1.0;                        // 电源始终激活
}

// 获取指定属性的值，优先查属性表，其次按电源类型返回
double PowerSource::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    if (key == "value") {
        return value_;
    }
    if (key == "sourceType") {
        return isVoltageSource() ? 0.0 : 1.0;
    }
    if (key == "voltage") {
        return isVoltageSource() ? value_ : 0.0;
    }
    if (key == "current") {
        return isCurrentSource() ? value_ : 0.0;
    }
    return 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> PowerSource::getAllProperties() const {
    return properties_;
}

// 克隆当前电源对象（复制设定值和类型）
Component* PowerSource::clone() const {
    return new PowerSource(id_, x_, y_, value_, sourceType_);
}

// 注册电源到元器件工厂，支持通过名称"PowerSource"创建实例
REGISTER_COMPONENT(PowerSource, PowerSource)

} // namespace CircuitSim