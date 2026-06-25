#ifndef COMPONENT_FACTORY_H
#define COMPONENT_FACTORY_H

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "Component.h"

namespace CircuitSim {

// 元件工厂(单例模式)
class ComponentFactory {
public:
    using Creator = std::function<Component*(int id, double x, double y)>;

    static ComponentFactory& getInstance();  // 获取单例实例

    void registerType(const std::string& typeName, Creator creator); // 注册元件类型
    Component* create(const std::string& typeName, int id, double x, double y); // 创建元件
    bool isTypeRegistered(const std::string& typeName) const; // 检查类型是否已注册

    static std::string typeToString(ComponentType type);  // 类型转字符串
    static ComponentType stringToType(const std::string& str); // 字符串转类型

    void registerBuiltInTypes();              // 注册内置类型
    static std::vector<std::string> getRegisteredTypes(); // 获取已注册类型列表

private:
    ComponentFactory() = default;
    std::map<std::string, Creator> creators_; // 类型创建器映射
};

#define REGISTER_COMPONENT(TYPE, CLASS) \
    static bool _reg_##CLASS = [](){ \
        CircuitSim::ComponentFactory::getInstance().registerType(#TYPE, \
            [](int id, double x, double y)->CircuitSim::Component*{ return new CLASS(id, x, y); }); \
        return true; \
    }();

} // namespace CircuitSim

#endif
