#ifndef COMPONENT_H
#define COMPONENT_H

#include <map>
#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace CircuitSim {

class Port;

// 元件类型枚举
enum class ComponentType {
    RESISTOR,     // 电阻
    POWER_SOURCE, // 电源
    BULB,         // 灯泡
    AMMETER,      // 电流表
    VOLTMETER,    // 电压表
    SWITCH,       // 开关
    CAPACITOR,    // 电容
    INDUCTOR      // 电感
};

// 电路元件基类
class Component {
protected:
    int id_;                        // 元件唯一标识符
    std::string name_;              // 元件名称
    double x_;                      // 画布X坐标
    double y_;                      // 画布Y坐标
    double rotation_;               // 旋转角度
    bool selected_;                 // 是否被选中
    std::vector<std::unique_ptr<Port>> ports_; // 端口列表
    std::map<std::string, double> properties_; // 属性键值对

public:
    Component(int id, const std::string& name, double x, double y);
    virtual ~Component();

    virtual ComponentType getType() const = 0;       // 获取元件类型
    virtual std::string getTypeName() const;         // 获取类型名称字符串
    virtual void simulate(double voltage, double current) = 0; // 更新仿真结果
    virtual double getProperty(const std::string& key) const = 0; // 获取指定属性
    virtual std::map<std::string, double> getAllProperties() const = 0; // 获取所有属性
    virtual Component* clone() const = 0;            // 克隆元件

    int getId() const { return id_; }
    std::string getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    void setProperty(const std::string& key, double value) { properties_[key] = value; }
    double getX() const { return x_; }
    double getY() const { return y_; }
    void setPosition(double x, double y) { x_ = x; y_ = y; }
    double getRotation() const { return rotation_; }
    void setRotation(double r) { rotation_ = r; }
    bool isSelected() const { return selected_; }
    void setSelected(bool s) { selected_ = s; }

    std::vector<Port*> getPorts() const;   // 获取所有端口
    Port* getPort(size_t index) const;     // 获取指定端口
    size_t portCount() const { return ports_.size(); }

    void addPort(Port* port);              // 添加端口

    virtual std::string toString() const;  // 字符串表示
};

} // namespace CircuitSim

#endif
