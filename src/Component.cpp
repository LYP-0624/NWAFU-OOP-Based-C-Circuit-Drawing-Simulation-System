#include "Component.h"

#include "ComponentFactory.h"
#include "Port.h"

namespace CircuitSim {

// 构造函数：初始化元器件ID、名称和坐标
Component::Component(int id, const std::string& name, double x, double y)
    : id_(id), name_(name), x_(x), y_(y), rotation_(0.0), selected_(false) {}

// 析构函数：使用默认实现（智能指针自动释放端口）
Component::~Component() = default;

// 获取元器件类型名称字符串
std::string Component::getTypeName() const {
    return ComponentFactory::typeToString(getType());
}

// 获取所有端口的原始指针列表
std::vector<Port*> Component::getPorts() const {
    std::vector<Port*> result;
    result.reserve(ports_.size());
    for (const auto& port : ports_) {
        result.push_back(port.get());
    }
    return result;
}

// 获取指定索引的端口，越界返回nullptr
Port* Component::getPort(size_t index) const {
    if (index >= ports_.size()) {
        return nullptr;
    }
    return ports_[index].get();
}

// 添加端口到元器件
void Component::addPort(Port* port) {
    if (port) {
        ports_.emplace_back(port);
    }
}

// 返回元器件的字符串描述
std::string Component::toString() const {
    return "Component[" + std::to_string(id_) + "] " + name_;
}

} // namespace CircuitSim