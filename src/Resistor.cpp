#include "Resistor.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化电阻，设置电阻值和初始状态，添加两个端口
Resistor::Resistor(int id, double x, double y, double resistance)
    : Component(id, "Resistor", x, y), resistance_(resistance), current_(0.0), voltage_(0.0) {
    addPort(new Port(this, 0));  // 正极端口
    addPort(new Port(this, 1));  // 负极端口
}

// 析构函数：使用默认实现
Resistor::~Resistor() = default;

// 返回元器件类型为电阻
ComponentType Resistor::getType() const {
    return ComponentType::RESISTOR;
}

// 仿真计算：记录电压电流，更新各项属性
void Resistor::simulate(double voltage, double current) {
    voltage_ = voltage;           // 记录两端电压
    current_ = current;           // 记录流过电流
    properties_["resistance"] = resistance_;   // 电阻值
    properties_["voltage"] = voltage_;         // 电压
    properties_["current"] = current_;         // 电流
    properties_["power"] = voltage_ * current_; // 消耗功率
    // 电流绝对值大于阈值时标记为激活状态
    properties_["isActive"] = (std::abs(current_) > 1e-12) ? 1.0 : 0.0;
}

// 获取指定属性的值，不存在则返回0
double Resistor::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> Resistor::getAllProperties() const {
    return properties_;
}

// 克隆当前电阻对象（复制电阻值）
Component* Resistor::clone() const {
    return new Resistor(id_, x_, y_, resistance_);
}

// 注册电阻到元器件工厂，支持通过名称"Resistor"创建实例
REGISTER_COMPONENT(Resistor, Resistor)

} // namespace CircuitSim