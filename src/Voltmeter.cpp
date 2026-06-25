#include "Voltmeter.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化电压表，设置初始读数，添加两个端口
Voltmeter::Voltmeter(int id, double x, double y)
    : Component(id, "Voltmeter", x, y), reading_(0.0) {
    addPort(new Port(this, 0));  // 正极端口
    addPort(new Port(this, 1));  // 负极端口
}

// 析构函数：使用默认实现
Voltmeter::~Voltmeter() = default;

// 返回元器件类型为电压表
ComponentType Voltmeter::getType() const {
    return ComponentType::VOLTMETER;
}

// 仿真计算：记录电压读数，更新各项属性
void Voltmeter::simulate(double voltage, double current) {
    reading_ = voltage;           // 电压读数等于两端电压
    properties_["resistance"] = 1e9;  // 电压表内阻极大（理想为无穷大）
    properties_["voltage"] = voltage;  // 两端电压
    properties_["current"] = current;  // 流过电流（极小）
    properties_["reading"] = reading_; // 显示读数
    properties_["power"] = voltage * current;  // 消耗功率
    properties_["isActive"] = 1.0;     // 标记为激活状态
}

// 获取指定属性的值，不存在则返回0
double Voltmeter::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> Voltmeter::getAllProperties() const {
    return properties_;
}

// 克隆当前电压表对象
Component* Voltmeter::clone() const {
    return new Voltmeter(id_, x_, y_);
}

// 注册电压表到元器件工厂，支持通过名称"Voltmeter"创建实例
REGISTER_COMPONENT(Voltmeter, Voltmeter)

} // namespace CircuitSim