#include "Ammeter.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化电流表，设置名称和初始读数，添加两个端口
Ammeter::Ammeter(int id, double x, double y)
    : Component(id, "Ammeter", x, y), reading_(0.0) {
    addPort(new Port(this, 0));  // 正极/流入端口
    addPort(new Port(this, 1));  // 负极/流出端口
}

// 析构函数：使用默认实现
Ammeter::~Ammeter() = default;

// 返回元器件类型为电流表
ComponentType Ammeter::getType() const {
    return ComponentType::AMMETER;
}

// 仿真计算：记录电流读数，更新各项属性
void Ammeter::simulate(double voltage, double current) {
    reading_ = current;  // 电流读数等于流过电流
    properties_["resistance"] = 0.001;  // 电流表内阻近似为0
    properties_["voltage"] = voltage;   // 两端电压
    properties_["current"] = current;   // 流过电流
    properties_["reading"] = reading_;  // 显示读数
    properties_["power"] = voltage * current;  // 消耗功率
    properties_["isActive"] = 1.0;      // 标记为激活状态
}

// 获取指定属性的值，不存在则返回0
double Ammeter::getProperty(const std::string& key) const {
    auto it = properties_.find(key);
    return (it != properties_.end()) ? it->second : 0.0;
}

// 获取所有属性的副本
std::map<std::string, double> Ammeter::getAllProperties() const {
    return properties_;
}

// 克隆当前电流表对象
Component* Ammeter::clone() const {
    return new Ammeter(id_, x_, y_);
}

// 注册电流表到元器件工厂，支持通过名称"Ammeter"创建实例
REGISTER_COMPONENT(Ammeter, Ammeter)

} // namespace CircuitSim