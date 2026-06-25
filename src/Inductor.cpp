#include "Inductor.h"

#include <cmath>

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化电感，设置电感值和初始状态，添加两个端口
Inductor::Inductor(int id, double x, double y, double inductance)
    : Component(id, "Inductor", x, y), inductance_(inductance),
      current_(0.0), voltage_(0.0) {
    addPort(new Port(this, 0));  // 正极端口
    addPort(new Port(this, 1));  // 负极端口
}

// 析构函数：使用默认实现
Inductor::~Inductor() = default;

// 返回元器件类型为电感
ComponentType Inductor::getType() const {
    return ComponentType::INDUCTOR;
}

// 仿真计算：记录电压电流，更新各项属性
void Inductor::simulate(double voltage, double current) {
    voltage_ = voltage;           // 记录两端电压
    current_ = current;           // 记录流过电流

    // 更新属性映射表
    properties_["inductance"] = inductance_;  // 电感值
    properties_["voltage"] = voltage_;        // 电压
    properties_["current"] = current_;        // 电流
    properties_["power"] = voltage_ * current_;  // 功率
    // 电流绝对值大于阈值时标记为激活状态
    properties_["isActive"] = (std::abs(current_) > 1e-10) ? 1.0 : 0.0;
}

// 获取指定属性的值，优先查属性表，其次查成员变量
double Inductor::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    if (it != properties_.end()) {
        return it->second;
    }
    if (key == "inductance") {
        return inductance_;
    }
    return 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> Inductor::getAllProperties() const {
    return properties_;
}

// 克隆当前电感对象（复制电感值）
Component* Inductor::clone() const {
    return new Inductor(id_, x_, y_, inductance_);
}

// 注册电感到元器件工厂，支持通过名称"Inductor"创建实例
REGISTER_COMPONENT(Inductor, Inductor)

} // namespace CircuitSim