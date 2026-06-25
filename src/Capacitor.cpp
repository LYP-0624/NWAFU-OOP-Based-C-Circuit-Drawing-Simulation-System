#include "Capacitor.h"

#include <cmath>

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化电容，设置电容值和初始状态，添加两个端口
Capacitor::Capacitor(int id, double x, double y, double capacitance)
    : Component(id, "Capacitor", x, y), capacitance_(capacitance),
      charge_(0.0), voltage_(0.0), current_(0.0) {
    addPort(new Port(this, 0));  // 正极端口
    addPort(new Port(this, 1));  // 负极端口
}

// 析构函数：使用默认实现
Capacitor::~Capacitor() = default;

// 返回元器件类型为电容
ComponentType Capacitor::getType() const {
    return ComponentType::CAPACITOR;
}

// 仿真计算：根据电压电流更新电荷量和功率
void Capacitor::simulate(double voltage, double current) {
    voltage_ = voltage;           // 记录两端电压
    current_ = current;           // 记录流过电流
    charge_ = capacitance_ * voltage_;  // 计算电荷量 Q = C * V

    // 更新属性映射表
    properties_["capacitance"] = capacitance_;  // 电容值
    properties_["voltage"] = voltage_;          // 电压
    properties_["current"] = current_;          // 电流
    properties_["charge"] = charge_;            // 电荷量
    properties_["power"] = voltage_ * current_; // 功率
    properties_["isActive"] = (std::abs(current_) > 1e-10) ? 1.0 : 0.0;  // 是否有电流通过
}

// 获取指定属性的值，优先查属性表，其次查成员变量
double Capacitor::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    if (key == "capacitance") {
        return capacitance_;
    }
    if (key == "charge") {
        return charge_;
    }
    return 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> Capacitor::getAllProperties() const {
    return properties_;
}

// 克隆当前电容对象（复制电容值）
Component* Capacitor::clone() const {
    return new Capacitor(id_, x_, y_, capacitance_);
}

// 注册电容到元器件工厂，支持通过名称"Capacitor"创建实例
REGISTER_COMPONENT(Capacitor, Capacitor)

} // namespace CircuitSim