#include "Switch.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化开关，设置初始状态和通断电阻，添加两个端口
Switch::Switch(int id, double x, double y, bool isOn)
    : Component(id, "Switch", x, y), isOn_(isOn), resistanceOn_(0.001), resistanceOff_(1e9) {
    addPort(new Port(this, 0));  // 正极端口
    addPort(new Port(this, 1));  // 负极端口
}

// 析构函数：使用默认实现
Switch::~Switch() = default;

// 返回元器件类型为开关
ComponentType Switch::getType() const {
    return ComponentType::SWITCH;
}

// 仿真计算：根据开关状态选择电阻值，更新各项属性
void Switch::simulate(double voltage, double current) {
    // 根据开关状态选择电阻：闭合时极小(0.001Ω)，断开时极大(1e9Ω)
    const double r = isOn_ ? resistanceOn_ : resistanceOff_;
    properties_["resistance"] = r;           // 当前等效电阻
    properties_["voltage"] = voltage;        // 两端电压
    properties_["current"] = current;        // 流过电流
    properties_["power"] = voltage * current; // 消耗功率
    properties_["isActive"] = isOn_ ? 1.0 : 0.0;  // 是否闭合
    properties_["state"] = isOn_ ? 1.0 : 0.0;     // 开关状态
    properties_["resistance_on"] = resistanceOn_;   // 闭合电阻
    properties_["resistance_off"] = resistanceOff_; // 断开电阻
}

// 获取指定属性的值，不存在则返回0
double Switch::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> Switch::getAllProperties() const {
    return properties_;
}

// 克隆当前开关对象（复制开关状态）
Component* Switch::clone() const {
    return new Switch(id_, x_, y_, isOn_);
}

// 注册开关到元器件工厂，支持通过名称"Switch"创建实例
REGISTER_COMPONENT(Switch, Switch)

} // namespace CircuitSim