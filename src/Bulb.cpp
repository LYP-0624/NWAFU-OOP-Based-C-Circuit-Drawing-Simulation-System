#include "Bulb.h"

#include <cmath>

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化灯泡，设置电阻、额定功率和初始状态，添加两个端口
Bulb::Bulb(int id, double x, double y, double resistance, double ratedPower)
    : Component(id, "Bulb", x, y), resistance_(resistance), ratedPower_(ratedPower),
      current_(0.0), voltage_(0.0), isLit_(false) {
    addPort(new Port(this, 0));  // 正极端口
    addPort(new Port(this, 1));  // 负极端口
}

// 析构函数：使用默认实现
Bulb::~Bulb() = default;

// 返回元器件类型为灯泡
ComponentType Bulb::getType() const {
    return ComponentType::BULB;
}

// 仿真计算：根据电压电流计算功率，判断是否发光
void Bulb::simulate(double voltage, double current) {
    voltage_ = voltage;           // 记录两端电压
    current_ = current;           // 记录流过电流
    const double power = std::abs(voltage_ * current_);  // 计算实际功率（取绝对值）
    // 功率大于阈值且电压有效时判定为点亮
    isLit_ = power > 1e-6 && std::abs(voltage_) > 0.01;

    // 更新属性映射表
    properties_["resistance"] = resistance_;   // 电阻值
    properties_["voltage"] = voltage_;         // 电压
    properties_["current"] = current_;         // 电流
    properties_["power"] = power;              // 功率
    properties_["isActive"] = isLit_ ? 1.0 : 0.0;  // 是否点亮
    properties_["ratedPower"] = ratedPower_;   // 额定功率
}

// 获取指定属性的值，不存在则返回0
double Bulb::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> Bulb::getAllProperties() const {
    return properties_;
}

// 克隆当前灯泡对象（复制电阻和额定功率）
Component* Bulb::clone() const {
    return new Bulb(id_, x_, y_, resistance_, ratedPower_);
}

// 注册灯泡到元器件工厂，支持通过名称"Bulb"创建实例
REGISTER_COMPONENT(Bulb, Bulb)

} // namespace CircuitSim